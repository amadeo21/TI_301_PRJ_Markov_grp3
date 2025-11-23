#include "adjlist.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ---------- utilitaires ---------- */

Cell* create_cell(int arrivee, float proba) {
    Cell *c = (Cell*)malloc(sizeof(Cell));
    if (!c) { perror("malloc cell"); exit(EXIT_FAILURE); }
    c->arrivee = arrivee;
    c->proba = proba;
    c->next = NULL;
    return c;
}

List create_empty_list(void) {
    List l;
    l.head = NULL;
    return l;
}

AdjList create_adjlist(int n) {
    AdjList g;
    g.n = n;
    g.lists = (List*)malloc((n+1) * sizeof(List)); // index 1..n
    if (!g.lists) { perror("malloc lists"); exit(EXIT_FAILURE); }
    for (int i = 1; i <= n; ++i) g.lists[i] = create_empty_list();
    return g;
}

void add_cell_to_list(List *l, int arrivee, float proba) {
    Cell *c = create_cell(arrivee, proba);
    /* on ajoute en tête (plus simple) */
    c->next = l->head;
    l->head = c;
}

void free_adjlist(AdjList *g) {
    if (!g || !g->lists) return;
    for (int i = 1; i <= g->n; ++i) {
        Cell *cur = g->lists[i].head;
        while (cur) {
            Cell *tmp = cur->next;
            free(cur);
            cur = tmp;
        }
    }
    free(g->lists);
    g->lists = NULL;
    g->n = 0;
}

void print_adjlist(const AdjList *g, FILE *out) {
    if (!g) return;
    for (int i = 1; i <= g->n; ++i) {
        fprintf(out, "Liste pour le sommet %d: [head] ", i);
        Cell *cur = g->lists[i].head;
        while (cur) {
            fprintf(out, "-> (%d, %.2f) ", cur->arrivee, cur->proba);
            cur = cur->next;
        }
        fprintf(out, "\n");
    }
}

/* getId : convertit 1 -> "A", 26 -> "Z", 27 -> "AA", ... */
void getId(int num, char *out, size_t out_sz) {
    if (num <= 0) { snprintf(out, out_sz, "?"); return; }
    // base 26 with 1-based digits
    char buf[32];
    int pos = 0;
    int n = num;
    while (n > 0) {
        int rem = (n - 1) % 26;
        buf[pos++] = 'A' + rem;
        n = (n - 1) / 26;
    }
    // buf contains reversed letters
    int j = 0;
    for (int i = pos - 1; i >= 0; --i) {
        if (j < (int)out_sz - 1) out[j++] = buf[i];
    }
    out[j] = '\0';
}

/* Lecture du graphe depuis un fichier */
AdjList readGraph(const char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) { perror("Could not open file for reading"); exit(EXIT_FAILURE); }
    int nbvert;
    if (fscanf(file, "%d", &nbvert) != 1) {
        fprintf(stderr, "Could not read number of vertices from %s\n", filename);
        fclose(file);
        exit(EXIT_FAILURE);
    }
    if (nbvert <= 0) {
        fprintf(stderr, "Invalid number of vertices: %d\n", nbvert);
        fclose(file);
        exit(EXIT_FAILURE);
    }
    AdjList g = create_adjlist(nbvert);
    int depart, arrivee;
    float proba;
    while (fscanf(file, "%d %d %f", &depart, &arrivee, &proba) == 3) {
        if (depart < 1 || depart > nbvert || arrivee < 1 || arrivee > nbvert) {
            fprintf(stderr, "Warning: ignoring invalid edge %d -> %d\n", depart, arrivee);
            continue;
        }
        add_cell_to_list(&g.lists[depart], arrivee, proba);
    }
    fclose(file);
    return g;
}

/* verify_markov: retourne 1 si graphe de Markov, 0 sinon.
   lower_bound et upper_bound définissent la tolérance (ex: 0.99 et 1.01) */
int verify_markov(const AdjList *g, float lower_bound, float upper_bound) {
    if (!g) return 0;
    int ok = 1;
    for (int i = 1; i <= g->n; ++i) {
        float sum = 0.0f;
        Cell *cur = g->lists[i].head;
        while (cur) { sum += cur->proba; cur = cur->next; }
        // tolérance numérique
        if (!(sum >= lower_bound && sum <= upper_bound)) {
            fprintf(stdout, "La somme des probabilités du sommet %d est %.6f (hors tolérance [%.3f, %.3f])\n",
                    i, sum, lower_bound, upper_bound);
            ok = 0;
        }
    }
    if (ok) {
        printf("Le graphe est un graphe de Markov (toutes les sommes sortantes dans [%.3f, %.3f]).\n",
               lower_bound, upper_bound);
    } else {
        printf("Le graphe n'est pas un graphe de Markov.\n");
    }
    return ok;
}

/* write_mermaid : génère un fichier texte Mermaid décrivant le graphe.
   Retourne 0 si ok, non-zero sinon. */
int write_mermaid(const AdjList *g, const char *filename) {
    if (!g || !filename) return -1;
    FILE *f = fopen(filename, "wt");
    if (!f) { perror("Could not open mermaid output file"); return -1; }

    fprintf(f, "---\nconfig:\n   layout: elk\n   theme: neo\n   look: neo\n---\n\n");
    fprintf(f, "flowchart LR\n");
    // déclarer les noeuds : A((1)) ...
    for (int i = 1; i <= g->n; ++i) {
        char id[8];
        getId(i, id, sizeof(id));
        fprintf(f, "%s((%d))\n", id, i);
    }
    fprintf(f, "\n");
    // écrire les arcs
    for (int i = 1; i <= g->n; ++i) {
        Cell *cur = g->lists[i].head;
        while (cur) {
            char id_from[8], id_to[8];
            getId(i, id_from, sizeof(id_from));
            getId(cur->arrivee, id_to, sizeof(id_to));
            fprintf(f, "%s -->|%.2f|%s\n", id_from, cur->proba, id_to);
            cur = cur->next;
        }
    }
    fclose(f);
    return 0;
}
