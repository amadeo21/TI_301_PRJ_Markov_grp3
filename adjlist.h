#ifndef ADJLIST_H
#define ADJLIST_H

#include <stdio.h>

typedef struct Cell {
    int arrivee;          // numéro du sommet d'arrivée (1..n)
    float proba;          // probabilité de la transition
    struct Cell *next;
} Cell;

typedef struct List {
    Cell *head;
} List;

typedef struct AdjList {
    List *lists;          // tableau dynamique de listes (indexé 1..n)
    int n;                // nombre de sommets
} AdjList;

/* Création / destruction */
Cell* create_cell(int arrivee, float proba);
List create_empty_list(void);
AdjList create_adjlist(int n);
void free_adjlist(AdjList *g);

/* Ajout / affichage */
void add_cell_to_list(List *l, int arrivee, float proba); /* ajoute en tête */
void print_adjlist(const AdjList *g, FILE *out);

/* Lecture du fichier (fonction demandée dans l'énoncé) */
AdjList readGraph(const char *filename);

/* Vérification Markov (somme des probabilités sortantes) */
int verify_markov(const AdjList *g, float lower_bound, float upper_bound);

/* Génération fichier Mermaid */
int write_mermaid(const AdjList *g, const char *filename);

/* conversion id -> label (A, B, ..., Z, AA, AB, ...) */
void getId(int num, char *out, size_t out_sz);

#endif /* ADJLIST_H */
