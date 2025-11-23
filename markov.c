// =====================================================
// markov.c — TI301 PARTIE 1
// Listes d'adjacence, vérification Markov, export Mermaid
// =====================================================

#include "markov.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------
// Internes : gestion de la liste chaînée
// -----------------------------------------------------

// Cellule : (sommet d'arrivée, probabilité, pointeur suivant)
static Cell* new_cell(int to, float p, Cell* next) {
    Cell* c = (Cell*)malloc(sizeof(Cell));
    if (!c) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    c->to = to;
    c->p  = p;
    c->next = next;
    return c;
}

static void list_init(List* L) { L->head = NULL; }

static void list_push_front(List* L, int to, float p) {
    L->head = new_cell(to, p, L->head);
}

// -----------------------------------------------------
// Construction / destruction de la liste d'adjacence
// -----------------------------------------------------

AdjList adj_create(int n) {
    AdjList G;
    G.n = n;
    G.adj = (List*)malloc((size_t)n * sizeof(List));
    if (!G.adj) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; ++i) list_init(&G.adj[i]);
    return G;
}

void adj_free(AdjList* G) {
    if (!G || !G->adj) return;

    for (int i = 0; i < G->n; ++i) {
        Cell* cur = G->adj[i].head;
        while (cur) {
            Cell* nxt = cur->next;
            free(cur);
            cur = nxt;
        }
    }
    free(G->adj);
    G->adj = NULL;
    G->n   = 0;
}

// -----------------------------------------------------
// Affichages
// -----------------------------------------------------

void adj_print(const AdjList* G) {
    for (int i = 0; i < G->n; ++i) {
        printf("Liste pour le sommet %d:", i + 1);
        const Cell* c = G->adj[i].head;
        printf(" [head @]");
        while (c) {
            printf(" -> (%d, %.2f)", c->to, c->p);
            c = c->next;
        }
        putchar('\n');
    }
}

// -----------------------------------------------------
// Lecture du graphe depuis un fichier texte
// -----------------------------------------------------

AdjList readGraph(const char* filename) {
    FILE* f = fopen(filename, "rt");
    if (!f) {
        perror("Could not open file for reading");
        exit(EXIT_FAILURE);
    }

    int n;
    if (fscanf(f, "%d", &n) != 1 || n <= 0) {
        fprintf(stderr, "Erreur: nombre de sommets invalide dans %s\n", filename);
        exit(EXIT_FAILURE);
    }
    AdjList G = adj_create(n);

    int   depart, arrivee;
    float proba;

    while (fscanf(f, "%d %d %f", &depart, &arrivee, &proba) == 3) {
        if (depart < 1 || depart > n || arrivee < 1 || arrivee > n) {
            fprintf(stderr,
                    "Avertissement: arc (%d -> %d) hors bornes [1..%d], ignoré.\n",
                    depart, arrivee, n);
            continue;
        }
        if (!(proba >= 0.0f && proba <= 1.0f)) {
            fprintf(stderr,
                    "Avertissement: probabilité %.6f hors [0,1], arc (%d -> %d) ignoré.\n",
                    proba, depart, arrivee);
            continue;
        }
        // insertion (arrivee, proba) dans la liste du sommet 'depart'
        list_push_front(&G.adj[depart - 1], arrivee, proba);
    }

    fclose(f);
    return G;
}

// -----------------------------------------------------
// Vérification de la propriété "graphe de Markov"
// -----------------------------------------------------

bool verify_markov(const AdjList* G, float lo, float hi) {
    bool ok = true;

    for (int i = 0; i < G->n; ++i) {
        double s = 0.0;
        for (Cell* c = G->adj[i].head; c; c = c->next) s += c->p;

        if (!(s >= lo && s <= hi)) {
            printf("Le graphe n’est pas un graphe de Markov : "
                   "la somme des probabilités du sommet %d est %.6f\n",
                   i + 1, s);
            ok = false;
        }
    }

    if (ok)
        printf("✅ Le graphe est un graphe de Markov (tolérance [%.2f, %.2f]).\n",
               lo, hi);

    return ok;
}

// -----------------------------------------------------
// Utilitaire : 1->"A", 2->"B", ..., 26->"Z", 27->"AA", ...
// -----------------------------------------------------

void getId(int num, char* buf, size_t sz) {
    char tmp[32];
    int  pos = 0;

    // Conversion style “tableur” (base 26, 1-indexée)
    while (num > 0 && pos < (int)sizeof(tmp) - 1) {
        num--;                       // décale pour base 26 1-indexée
        tmp[pos++] = (char)('A' + (num % 26));
        num /= 26;
    }

    // renverse la chaîne
    int j = 0;
    for (int i = pos - 1; i >= 0 && j < (int)sz - 1; --i) buf[j++] = tmp[i];
    buf[j] = '\0';
}

// -----------------------------------------------------
// Export au format Mermaid
// -----------------------------------------------------

bool write_mermaid(const AdjList* G, const char* outpath) {
    FILE* out = fopen(outpath, "wt");
    if (!out) {
        perror("open mermaid");
        return false;
    }

    // En-tête de config et début du flowchart
    fprintf(out,
            "---\n"
            "config:\n"
            "   layout: elk\n"
            "   theme: neo\n"
            "   look: neo\n"
            "---\n\n"
            "flowchart LR\n");

    // Déclaration des nœuds (A((1)), B((2)), ...)
    for (int i = 0; i < G->n; ++i) {
        char id[32];
        getId(i + 1, id, sizeof id);
        fprintf(out, "%s((%d))\n", id, i + 1);
    }
    fputc('\n', out);

    // Arêtes avec probabilités (A -->|0.95|A, etc.)
    for (int i = 0; i < G->n; ++i) {
        char A[32];
        getId(i + 1, A, sizeof A);
        for (Cell* c = G->adj[i].head; c; c = c->next) {
            char B[32];
            getId(c->to, B, sizeof B);
            fprintf(out, "%s -->|%.2f|%s\n", A, c->p, B);
        }
    }

    fclose(out);
    return true;
}