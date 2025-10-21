#ifndef MARKOV_H
#define MARKOV_H

#include <stdbool.h>
#include <stddef.h>

typedef struct Cell {
    int to;
    float p;
    struct Cell* next;
} Cell;

typedef struct {
    Cell* head;
} List;

typedef struct {
    int n;      // sommets numérotés 1..n
    List* adj;  // tableau de listes de taille n
} AdjList;

// Construction / destruction
AdjList adj_create(int n);
void    adj_free(AdjList* G);

// I/O
AdjList readGraph(const char* filename);  // lit le format sujet
void    adj_print(const AdjList* G);

// Vérification Markov (tolérance incluse)
bool    verify_markov(const AdjList* G, float lo, float hi);

// Export Mermaid (.mmd)
bool    write_mermaid(const AdjList* G, const char* outpath);

// Util: 1->"A", 2->"B", ..., 26->"Z", 27->"AA", etc.
void    getId(int num, char* buf, size_t sz);

#endif