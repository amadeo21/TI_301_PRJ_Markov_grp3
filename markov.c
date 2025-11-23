#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "markov.h"
#include "utils.h"

/* ====================== OUTILS INTERNES ====================== */

static t_arc *create_arc(int dest, float proba)
{
    t_arc *a = malloc(sizeof(t_arc));
    if (!a) {
        perror("malloc arc");
        exit(EXIT_FAILURE);
    }
    a->dest = dest;
    a->proba = proba;
    a->next = NULL;
    return a;
}

static void add_arc(t_graph *g, int src, int dest, float proba)
{
    if (src < 1 || src > g->nb_vertices) return;
    t_arc *new_arc = create_arc(dest, proba);
    new_arc->next = g->array[src - 1].head;
    g->array[src - 1].head = new_arc;
}

/* ====================== PARTIE 1 : GRAPHE ====================== */

t_graph *read_graph(const char *filename)
{
    FILE *f = fopen(filename, "rt");
    if (!f) {
        perror("open graph file");
        return NULL;
    }

    int n;
    if (fscanf(f, "%d", &n) != 1) {
        fprintf(stderr, "Erreur lecture nb sommets\n");
        fclose(f);
        return NULL;
    }

    t_graph *g = malloc(sizeof(t_graph));
    if (!g) {
        perror("malloc graph");
        fclose(f);
        return NULL;
    }
    g->nb_vertices = n;
    g->array = calloc((size_t)n, sizeof(t_adj_node));
    if (!g->array) {
        perror("calloc adj array");
        fclose(f);
        free(g);
        return NULL;
    }

    int depart, arrivee;
    float proba;
    while (fscanf(f, "%d %d %f", &depart, &arrivee, &proba) == 3) {
        add_arc(g, depart, arrivee, proba);
    }

    fclose(f);
    return g;
}

void free_graph(t_graph *g)
{
    if (!g) return;
    for (int i = 0; i < g->nb_vertices; ++i) {
        t_arc *cur = g->array[i].head;
        while (cur) {
            t_arc *tmp = cur->next;
            free(cur);
            cur = tmp;
        }
    }
    free(g->array);
    free(g);
}

void print_adj_list(const t_graph *g)
{
    if (!g) return;
    printf("Graph (%d sommets)\n", g->nb_vertices);
    for (int i = 0; i < g->nb_vertices; ++i) {
        printf("Sommet %d: ", i + 1);
        t_arc *cur = g->array[i].head;
        while (cur) {
            printf("-> (%d, %.2f) ", cur->dest, cur->proba);
            cur = cur->next;
        }
        printf("\n");
    }
}

int check_markov_graph(const t_graph *g, float eps)
{
    if (!g) return 0;
    int ok = 1;
    for (int i = 0; i < g->nb_vertices; ++i) {
        float sum = 0.0f;
        for (t_arc *cur = g->array[i].head; cur; cur = cur->next) {
            sum += cur->proba;
        }
        if (sum < 1.0f - eps || sum > 1.0f + eps) {
            printf("Sommet %d : somme des proba = %.4f (pas dans [1-eps,1+eps])\n",
                   i + 1, sum);
            ok = 0;
        }
    }
    if (ok)
        printf("Le graphe est un graphe de Markov (eps=%.3f)\n", eps);
    else
        printf("Le graphe n'est pas un graphe de Markov (eps=%.3f)\n", eps);
    return ok;
}

int export_mermaid_graph(const t_graph *g, const char *filename)
{
    if (!g || !filename) return 0;
    FILE *f = fopen(filename, "wt");
    if (!f) {
        perror("open mermaid graph file");
        return 0;
    }

    fprintf(f,
            "---\n"
            "config:\n"
            "   layout: elk\n"
            "   theme: neo\n"
            "   look: neo\n"
            "---\n\n");
    fprintf(f, "flowchart LR\n");

    // déclarations des sommets
    for (int i = 0; i < g->nb_vertices; ++i) {
        char *id = getId(i + 1);
        fprintf(f, "%s((%d))\n", id, i + 1);
        free(id);
    }
    fprintf(f, "\n");

    // arêtes
    for (int i = 0; i < g->nb_vertices; ++i) {
        char *from_id = getId(i + 1);
        for (t_arc *cur = g->array[i].head; cur; cur = cur->next) {
            char *to_id = getId(cur->dest);
            fprintf(f, "%s -->|%.2f|%s\n", from_id, cur->proba, to_id);
            free(to_id);
        }
        free(from_id);
    }

    fclose(f);
    return 1;
}

/* ====================== PARTIE 2 : TARJAN ====================== */

static void init_partition(t_partition *p)
{
    p->size = 0;
    p->capacity = 4;
    p->classes = malloc(sizeof(t_class) * p->capacity);
    if (!p->classes) {
        perror("malloc partition");
        exit(EXIT_FAILURE);
    }
}

static void class_add_vertex(t_class *c, int v)
{
    if (c->capacity == 0) {
        c->capacity = 4;
        c->vertices = malloc(sizeof(int) * c->capacity);
    } else if (c->size >= c->capacity) {
        c->capacity *= 2;
        int *tmp = realloc(c->vertices, sizeof(int) * c->capacity);
        if (!tmp) {
            perror("realloc class vertices");
            exit(EXIT_FAILURE);
        }
        c->vertices = tmp;
    }
    c->vertices[c->size++] = v;
}

static void partition_add_class(t_partition *p, const int *verts, int nb_verts, int class_index)
{
    if (p->size >= p->capacity) {
        p->capacity *= 2;
        t_class *tmp = realloc(p->classes, sizeof(t_class) * p->capacity);
        if (!tmp) {
            perror("realloc partition");
            exit(EXIT_FAILURE);
        }
        p->classes = tmp;
    }
    t_class *c = &p->classes[p->size];
    snprintf(c->name, sizeof(c->name), "C%d", class_index);
    c->size = 0;
    c->capacity = nb_verts > 0 ? nb_verts : 1;
    c->vertices = malloc(sizeof(int) * c->capacity);
    if (!c->vertices) {
        perror("malloc class vertices");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < nb_verts; ++i) {
        c->vertices[i] = verts[i];
    }
    c->size = nb_verts;
    p->size++;
}

/* Tarjan récursif */
static void tarjan_visit(int v_index,
                         const t_graph *g,
                         t_tarjan_vertex *verts,
                         t_int_stack *stack,
                         int *current_index,
                         t_partition *part,
                         int *class_counter)
{
    t_tarjan_vertex *v = &verts[v_index];
    v->index = *current_index;
    v->lowlink = *current_index;
    (*current_index)++;

    stack_push(stack, v_index);
    v->on_stack = 1;

    // Parcours des successeurs
    t_arc *cur = g->array[v_index].head;
    while (cur) {
        int w_index = cur->dest - 1;
        t_tarjan_vertex *w = &verts[w_index];
        if (w->index == -1) {
            tarjan_visit(w_index, g, verts, stack, current_index, part, class_counter);
            if (w->lowlink < v->lowlink)
                v->lowlink = w->lowlink;
        } else if (w->on_stack) {
            if (w->index < v->lowlink)
                v->lowlink = w->index;
        }
        cur = cur->next;
    }

    // racine d'une SCC
    if (v->lowlink == v->index) {
        int w_idx;
        int scc_verts_capacity = 4;
        int scc_verts_size = 0;
        int *scc_verts = malloc(sizeof(int) * scc_verts_capacity);
        if (!scc_verts) {
            perror("malloc scc_verts");
            exit(EXIT_FAILURE);
        }

        do {
            w_idx = stack_pop(stack);
            verts[w_idx].on_stack = 0;
            if (scc_verts_size >= scc_verts_capacity) {
                scc_verts_capacity *= 2;
                int *tmp = realloc(scc_verts, sizeof(int) * scc_verts_capacity);
                if (!tmp) {
                    perror("realloc scc_verts");
                    exit(EXIT_FAILURE);
                }
                scc_verts = tmp;
            }
            scc_verts[scc_verts_size++] = verts[w_idx].id;
        } while (w_idx != v_index);

        (*class_counter)++;
        partition_add_class(part, scc_verts, scc_verts_size, *class_counter);
        free(scc_verts);
    }
}

t_partition tarjan_partition(const t_graph *g)
{
    t_partition part;
    init_partition(&part);

    if (!g) return part;

    int n = g->nb_vertices;
    t_tarjan_vertex *verts = malloc(sizeof(t_tarjan_vertex) * n);
    if (!verts) {
        perror("malloc tarjan verts");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; ++i) {
        verts[i].id = i + 1;
        verts[i].index = -1;
        verts[i].lowlink = -1;
        verts[i].on_stack = 0;
    }

    t_int_stack *stack = stack_create(n);
    int current_index = 0;
    int class_counter = 0;

    for (int i = 0; i < n; ++i) {
        if (verts[i].index == -1) {
            tarjan_visit(i, g, verts, stack, &current_index, &part, &class_counter);
        }
    }

    stack_free(stack);
    free(verts);
    return part;
}

void free_partition(t_partition *p)
{
    if (!p || !p->classes) return;
    for (int i = 0; i < p->size; ++i) {
        free(p->classes[i].vertices);
    }
    free(p->classes);
    p->classes = NULL;
    p->size = 0;
    p->capacity = 0;
}

void print_partition(const t_partition *p)
{
    if (!p) return;
    for (int i = 0; i < p->size; ++i) {
        const t_class *c = &p->classes[i];
        printf("Composante %s: {", c->name);
        for (int j = 0; j < c->size; ++j) {
            printf("%d", c->vertices[j]);
            if (j + 1 < c->size) printf(", ");
        }
        printf("}\n");
    }
}

int *build_vertex_to_class(const t_partition *p, int nb_vertices)
{
    int *arr = calloc_int_array(nb_vertices);
    // arr[i] = indice de la classe contenant le sommet i+1
    for (int ci = 0; ci < p->size; ++ci) {
        const t_class *c = &p->classes[ci];
        for (int j = 0; j < c->size; ++j) {
            int v = c->vertices[j];
            if (v >= 1 && v <= nb_vertices) {
                arr[v - 1] = ci;
            }
        }
    }
    return arr;
}

/* ====================== PARTIE 3 : MATRICES ====================== */

static float **alloc_matrix_data(int n)
{
    float **data = malloc(sizeof(float *) * n);
    if (!data) {
        perror("malloc matrix rows");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; ++i) {
        data[i] = calloc((size_t)n, sizeof(float));
        if (!data[i]) {
            perror("calloc matrix row");
            exit(EXIT_FAILURE);
        }
    }
    return data;
}

t_matrix create_empty_matrix(int n)
{
    t_matrix M;
    M.rows = n;
    M.cols = n;
    M.data = alloc_matrix_data(n);
    return M;
}

t_matrix matrix_from_graph(const t_graph *g)
{
    int n = g->nb_vertices;
    t_matrix M = create_empty_matrix(n);
    for (int i = 0; i < n; ++i) {
        for (t_arc *cur = g->array[i].head; cur; cur = cur->next) {
            int j = cur->dest - 1;
            M.data[i][j] = cur->proba;
        }
    }
    return M;
}

void free_matrix(t_matrix *m)
{
    if (!m || !m->data) return;
    for (int i = 0; i < m->rows; ++i) {
        free(m->data[i]);
    }
    free(m->data);
    m->data = NULL;
    m->rows = m->cols = 0;
}

void copy_matrix(t_matrix *dst, const t_matrix *src)
{
    if (!dst || !src || dst->rows != src->rows || dst->cols != src->cols) return;
    for (int i = 0; i < src->rows; ++i) {
        for (int j = 0; j < src->cols; ++j) {
            dst->data[i][j] = src->data[i][j];
        }
    }
}

void multiply_matrices(const t_matrix *A, const t_matrix *B, t_matrix *C)
{
    int n = A->rows;
    if (A->cols != n || B->rows != n || B->cols != n ||
        C->rows != n || C->cols != n) {
        fprintf(stderr, "Erreur dimensions multiplication matrices\n");
        return;
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < n; ++k) {
                sum += A->data[i][k] * B->data[k][j];
            }
            C->data[i][j] = sum;
        }
    }
}

float diff_matrices(const t_matrix *M, const t_matrix *N)
{
    if (!M || !N || M->rows != N->rows || M->cols != N->cols) {
        fprintf(stderr, "diff_matrices: dimensions incompatibles\n");
        return 0.0f;
    }
    float sum = 0.0f;
    for (int i = 0; i < M->rows; ++i) {
        for (int j = 0; j < M->cols; ++j) {
            sum += fabsf(M->data[i][j] - N->data[i][j]);
        }
    }
    return sum;
}

t_matrix matrix_power(const t_matrix *M, int power)
{
    int n = M->rows;
    t_matrix result = create_empty_matrix(n);
    t_matrix tmp = create_empty_matrix(n);

    // result = identité
    for (int i = 0; i < n; ++i) {
        result.data[i][i] = 1.0f;
    }

    // copie de M
    t_matrix base = create_empty_matrix(n);
    copy_matrix(&base, M);

    for (int p = 0; p < power; ++p) {
        multiply_matrices(&result, &base, &tmp);
        copy_matrix(&result, &tmp);
    }

    free_matrix(&base);
    free_matrix(&tmp);
    return result;
}

/* Sous-matrice pour une classe */

t_matrix subMatrix(t_matrix matrix, t_partition part, int compo_index)
{
    if (compo_index < 0 || compo_index >= part.size) {
        fprintf(stderr, "subMatrix: indice de composante invalide\n");
        t_matrix empty = {0, 0, NULL};
        return empty;
    }

    t_class *c = &part.classes[compo_index];
    int k = c->size;
    t_matrix sub = create_empty_matrix(k);

    // On copie les lignes/colonnes correspondantes
    for (int i = 0; i < k; ++i) {
        int vi = c->vertices[i] - 1;
        for (int j = 0; j < k; ++j) {
            int vj = c->vertices[j] - 1;
            sub.data[i][j] = matrix.data[vi][vj];
        }
    }
    return sub;
}

/* Distribution stationnaire (approx) */

float *stationary_distribution(const t_matrix *M, float eps, int max_iter)
{
    int n = M->rows;
    t_matrix prev = create_empty_matrix(n);
    t_matrix cur  = create_empty_matrix(n);

    // prev = M
    copy_matrix(&prev, M);
    copy_matrix(&cur, M);

    for (int iter = 0; iter < max_iter; ++iter) {
        t_matrix next = create_empty_matrix(n);
        multiply_matrices(&cur, M, &next);
        float d = diff_matrices(&cur, &next);
        free_matrix(&prev);
        prev = cur;
        cur = next;

        if (d < eps) {
            break;
        }
    }

    // On prend la première ligne comme distribution stationnaire approximative
    float *dist = calloc_float_array(n);
    for (int j = 0; j < n; ++j) {
        dist[j] = cur.data[0][j];
    }

    free_matrix(&prev);
    free_matrix(&cur);
    return dist;
}

/* ====================== PÉRIODICITÉ (BONUS) ====================== */

static int gcd_int(int a, int b)
{
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

static int gcd_array(int *vals, int nbvals)
{
    if (nbvals == 0) return 0;
    int result = vals[0];
    for (int i = 1; i < nbvals; ++i) {
        result = gcd_int(result, vals[i]);
    }
    return result;
}

int getPeriod(t_matrix sub_matrix)
{
    int n = sub_matrix.rows;
    int *periods = malloc(n * sizeof(int));
    if (!periods) {
        perror("malloc periods");
        exit(EXIT_FAILURE);
    }
    int period_count = 0;
    int cpt;

    t_matrix power_matrix = create_empty_matrix(n);
    t_matrix result_matrix = create_empty_matrix(n);
    copy_matrix(&power_matrix, &sub_matrix);

    for (cpt = 1; cpt <= n; cpt++)
    {
        int diag_nonzero = 0;
        for (int i = 0; i < n; i++)
        {
            if (power_matrix.data[i][i] > 0.0f)
            {
                diag_nonzero = 1;
                break;
            }
        }
        if (diag_nonzero) {
            periods[period_count] = cpt;
            period_count++;
        }
        multiply_matrices(&power_matrix, &sub_matrix, &result_matrix);
        copy_matrix(&power_matrix, &result_matrix);
    }

    int period = gcd_array(periods, period_count);

    free(periods);
    free_matrix(&power_matrix);
    free_matrix(&result_matrix);
    return period;
}
