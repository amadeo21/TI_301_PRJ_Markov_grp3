#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "utils.h"

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

t_graph *readGraph(const char *filename)
{
    FILE *f = fopen(filename, "rt");

    int n;
    if (fscanf(f, "%d", &n) != 1) {
        fprintf(stderr, "Erreur lecture nb sommets\n");
        fclose(f);
        return NULL;
    }

    t_graph *g = malloc(sizeof(t_graph));

    g->nb_vertices = n;
    g->array = calloc((size_t)n, sizeof(t_adj_list));

    int depart, arrivee;
    float proba;
    while (fscanf(f, "%d %d %f", &depart, &arrivee, &proba) == 3) {
        add_arc(g, depart, arrivee, proba);
    }

    fclose(f);
    return g;
}

void freeGraph(t_graph *g)
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

void printAdjList(const t_graph *g)
{
    if (!g) return;
    printf("Graphe (%d sommets)\n", g->nb_vertices);
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

int checkMarkov(const t_graph *g, float eps)
{
    if (!g) return 0;
    int ok = 1;
    for (int i = 0; i < g->nb_vertices; ++i) {
        float sum = 0.0f;
        for (t_arc *cur = g->array[i].head; cur; cur = cur->next) {
            sum += cur->proba;
        }
        if (sum < 1.0f - eps || sum > 1.0f + eps) {
            printf("Sommet %d : somme des probabilites = %.4f (hors [1-eps,1+eps])\n",
                   i + 1, sum);
            ok = 0;
        }
    }
    if (ok)
        printf("Le graphe est un graphe de Markov (eps=%.3f)\n", eps);
    else
        printf("Le graphe N'EST PAS un graphe de Markov (eps=%.3f)\n", eps);
    return ok;
}

int exportMermaidGraph(const t_graph *g, const char *filename)
{
    if (!g || !filename) return 0;
    FILE *f = fopen(filename, "wt");
    if (!f) {
        perror("open mermaid graph file");
        return 0;
    }

    fprintf(f, "flowchart LR\n");

    for (int i = 0; i < g->nb_vertices; ++i) {
        char *id = getId(i + 1);
        fprintf(f, "    %s((%d))\n", id, i + 1);
        free(id);
    }
    fprintf(f, "\n");

    for (int i = 0; i < g->nb_vertices; ++i) {
        char *from_id = getId(i + 1);
        for (t_arc *cur = g->array[i].head; cur; cur = cur->next) {
            char *to_id = getId(cur->dest);
            fprintf(f, "    %s -->|%.2f|%s\n", from_id, cur->proba, to_id);
            free(to_id);
        }
        free(from_id);
    }

    fclose(f);
    return 1;
}
