#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tarjan.h"
#include "utils.h"

static void init_partition(t_partition *p)
{
    p->size = 0;
    p->capacity = 4;
    p->classes = malloc(sizeof(t_class) * (size_t)p->capacity);
    if (!p->classes) {
        perror("malloc partition");
        exit(EXIT_FAILURE);
    }
}

static void partition_add_class(t_partition *p, const int *verts, int nb_verts, int class_index)
{
    if (p->size >= p->capacity) {
        p->capacity *= 2;
        t_class *tmp = realloc(p->classes, sizeof(t_class) * (size_t)p->capacity);
        if (!tmp) {
            perror("realloc partition");
            exit(EXIT_FAILURE);
        }
        p->classes = tmp;
    }
    t_class *c = &p->classes[p->size];
    snprintf(c->name, sizeof(c->name), "C%d", class_index);
    c->size = nb_verts;
    c->capacity = nb_verts > 0 ? nb_verts : 1;
    c->vertices = malloc(sizeof(int) * (size_t)c->capacity);
    if (!c->vertices) {
        perror("malloc class vertices");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < nb_verts; ++i) {
        c->vertices[i] = verts[i];
    }
    p->size++;
}

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

    if (v->lowlink == v->index) {
        int w_idx;
        int capacity = 4;
        int size = 0;
        int *scc_verts = malloc(sizeof(int) * (size_t)capacity);
        if (!scc_verts) {
            perror("malloc scc_verts");
            exit(EXIT_FAILURE);
        }

        do {
            w_idx = stack_pop(stack);
            verts[w_idx].on_stack = 0;
            if (size >= capacity) {
                capacity *= 2;
                int *tmp = realloc(scc_verts, sizeof(int) * (size_t)capacity);
                if (!tmp) {
                    perror("realloc scc_verts");
                    exit(EXIT_FAILURE);
                }
                scc_verts = tmp;
            }
            scc_verts[size++] = verts[w_idx].id;
        } while (w_idx != v_index);

        (*class_counter)++;
        partition_add_class(part, scc_verts, size, *class_counter);
        free(scc_verts);
    }
}

t_partition tarjanPartition(const t_graph *g)
{
    t_partition part;
    init_partition(&part);

    if (!g) return part;

    int n = g->nb_vertices;
    t_tarjan_vertex *verts = malloc(sizeof(t_tarjan_vertex) * (size_t)n);
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

void freePartition(t_partition *p)
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

void printPartition(const t_partition *p)
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

int *buildVertexToClass(const t_partition *p, int nb_vertices)
{
    int *arr = calloc_int_array(nb_vertices);
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
