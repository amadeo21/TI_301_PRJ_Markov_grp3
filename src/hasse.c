#include <stdio.h>
#include <stdlib.h>
#include "hasse.h"
#include "utils.h"

void init_link_array(t_link_array *arr)
{
    arr->size = 0;
    arr->capacity = 4;
    arr->data = malloc(sizeof(t_link) * (size_t)arr->capacity);

}

void free_link_array(t_link_array *arr)
{
    if (!arr || !arr->data) return;
    free(arr->data);
    arr->data = NULL;
    arr->size = 0;
    arr->capacity = 0;
}

static int link_exists(const t_link_array *arr, int from, int to)
{
    for (int i = 0; i < arr->size; ++i) {
        if (arr->data[i].from == from && arr->data[i].to == to)
            return 1;
    }
    return 0;
}

void add_link_unique(t_link_array *arr, int from, int to)
{
    if (from == to) return;
    if (link_exists(arr, from, to)) return;

    if (arr->size >= arr->capacity) {
        arr->capacity *= 2;
        t_link *tmp = realloc(arr->data, sizeof(t_link) * (size_t)arr->capacity)
        arr->data = tmp;
    }
    arr->data[arr->size].from = from;
    arr->data[arr->size].to = to;
    arr->size++;
}

void build_class_links(const t_graph *g, const t_partition *part, t_link_array *links)
{
    int n = g->nb_vertices;
    int *vertex_to_class = buildVertexToClass(part, n);

    for (int i = 0; i < n; ++i) {
        int Ci = vertex_to_class[i];
        for (t_arc *cur = g->array[i].head; cur; cur = cur->next) {
            int j = cur->dest - 1;
            int Cj = vertex_to_class[j];
            if (Ci != Cj) {
                add_link_unique(links, Ci, Cj);
            }
        }
    }

    free(vertex_to_class);
}

void removeTransitiveLinks(t_link_array *links)
{
    int changed = 1;
    while (changed) {
        changed = 0;
        for (int i = 0; i < links->size; ++i) {
            t_link l1 = links->data[i];
            if (l1.from == -1) continue;

            for (int j = 0; j < links->size; ++j) {
                t_link l2 = links->data[j];
                if (l2.from == -1) continue;
                if (l1.to == l2.from) {
                    for (int k = 0; k < links->size; ++k) {
                        t_link l3 = links->data[k];
                        if (l3.from == -1) continue;
                        if (l3.from == l1.from && l3.to == l2.to) {
                            links->data[k].from = -1;
                            changed = 1;
                        }
                    }
                }
            }
        }
    }

    int w = 0;
    for (int i = 0; i < links->size; ++i) {
        if (links->data[i].from != -1) {
            links->data[w++] = links->data[i];
        }
    }
    links->size = w;
}

int export_mermaid_hasse(const t_partition *part, const t_link_array *links, const char *filename)
{
    FILE *f = fopen(filename, "wt");
    if (!f) {
        perror("open mermaid hasse file");
        return 0;
    }

    fprintf(f, "flowchart TB\n");

    for (int i = 0; i < part->size; ++i) {
        const t_class *c = &part->classes[i];
        fprintf(f, "    C%d(\"%s\\n{", i, c->name);
        for (int j = 0; j < c->size; ++j) {
            fprintf(f, "%d", c->vertices[j]);
            if (j + 1 < c->size) fprintf(f, ",");
        }
        fprintf(f, "}\")\n");
    }
    fprintf(f, "\n");

    for (int i = 0; i < links->size; ++i) {
        int from = links->data[i].from;
        int to   = links->data[i].to;
        fprintf(f, "    C%d --> C%d\n", from, to);
    }

    fclose(f);
    return 1;
}

void classify_graph(const t_graph *g, const t_partition *part, const t_link_array *links)
{
    if (!g || !part || !links) return;

    int nb_classes = part->size;
    int *has_outgoing = calloc((size_t)nb_classes, sizeof(int));
    if (!has_outgoing) {
        perror("calloc has_outgoing");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < links->size; ++i) {
        int from = links->data[i].from;
        if (from >= 0 && from < nb_classes) {
            has_outgoing[from] = 1;
        }
    }

    printf("\n=== Caracteristiques des classes ===\n");
    for (int ci = 0; ci < nb_classes; ++ci) {
        const t_class *c = &part->classes[ci];
        int transient = has_outgoing[ci];
        int persistent = !transient;
        printf("%s: {", c->name);
        for (int j = 0; j < c->size; ++j) {
            printf("%d", c->vertices[j]);
            if (j + 1 < c->size) printf(", ");
        }
        printf("} -> ");
        if (persistent) printf("classe persistante");
        else printf("classe transitoire");
        if (persistent && c->size == 1) {
            printf(" (etat absorbant)");
        }
        printf("\n");
    }

    if (nb_classes == 1) {
        printf("\nLe graphe de Markov est irreductible (une seule classe).\n");
    } else {
        printf("\nLe graphe de Markov n'est pas irreductible.\n");
    }

    free(has_outgoing);
}
