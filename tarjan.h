#ifndef TARJAN_H
#define TARJAN_H

#include "adjlist.h"

/* Structure représentant une classe (composante fortement connexe) */
typedef struct {
    int *vertices;    /* tableau dynamique des sommets */
    int size;
    int capacity;
    char name[16];    /* ex "C1" */
} t_class;

/* Partition : ensemble de classes */
typedef struct {
    t_class *classes;
    int count;
    int capacity;
} t_partition;

/* Résultat utile : table vertex -> index de la classe (1..count), 0 = not set */
typedef struct {
    t_partition partition;
    int *vertex_to_class; /* size n+1, indexé 1..n */
} t_partition_result;

/* Fonctions */
void init_t_class(t_class *c);
void free_t_class(t_class *c);

void init_t_partition(t_partition *p);
void free_t_partition(t_partition *p);
void add_vertex_to_class(t_class *c, int v);
void add_class_to_partition(t_partition *p, t_class *c);

t_partition_result tarjan_partition(const AdjList *g);
/* free helper */
void free_partition_result(t_partition_result *res);

#endif /* TARJAN_H */
