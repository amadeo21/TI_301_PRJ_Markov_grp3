#ifndef TARJAN_H
#define TARJAN_H

#include "graph.h"

/**
 * @brief Structure utilisée par l'algorithme de Tarjan pour un sommet.
 */
typedef struct {
    int id;         /**< Identifiant du sommet (1..n). */
    int index;      /**< Numéro de visite (DFS), -1 si non visité. */
    int lowlink;    /**< Plus petit index accessible. */
    int on_stack;   /**< Booléen (0/1) indiquant si le sommet est sur la pile. */
} t_tarjan_vertex;

/**
 * @brief Classe (composante fortement connexe).
 */
typedef struct {
    char name[16];  /**< Nom symbolique, ex : "C1". */
    int *vertices;  /**< Sommets (1..n). */
    int size;
    int capacity;
} t_class;

/**
 * @brief Partition du graphe en classes.
 */
typedef struct {
    t_class *classes;
    int size;
    int capacity;
} t_partition;

/**
 * @brief Calcule la partition du graphe en composantes fortement connexes.
 */
t_partition tarjanPartition(const t_graph *g);

/**
 * @brief Libère la mémoire d'une partition.
 */
void freePartition(t_partition *p);

/**
 * @brief Affiche la partition.
 */
void printPartition(const t_partition *p);

/**
 * @brief Construit un tableau: pour chaque sommet -> indice de la classe (0..size-1).
 */
int *buildVertexToClass(const t_partition *p, int nb_vertices);

#endif // TARJAN_H
