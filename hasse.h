#ifndef HASSE_H
#define HASSE_H

#include "markov.h"

/**
 * @brief Lien entre deux classes (pour le diagramme de Hasse).
 */
typedef struct {
    int from;   /**< indice de la classe de départ. */
    int to;     /**< indice de la classe d'arrivée. */
} t_link;

/**
 * @brief Tableau dynamique de liens.
 */
typedef struct {
    t_link *data;
    int size;
    int capacity;
} t_link_array;

/**
 * @brief Initialise un tableau de liens vide.
 */
void init_link_array(t_link_array *arr);

/**
 * @brief Libère la mémoire d'un tableau de liens.
 */
void free_link_array(t_link_array *arr);

/**
 * @brief Ajoute un lien (from,to) si non déjà présent.
 */
void add_link_unique(t_link_array *arr, int from, int to);

/**
 * @brief Construit l'ensemble des liens entre classes à partir du graphe et de la partition.
 */
void build_class_links(const t_graph *g, const t_partition *part, t_link_array *links);

/**
 * @brief Supprime les liens transitifs (diagramme de Hasse propre).
 */
void removeTransitiveLinks(t_link_array *links);

/**
 * @brief Exporte le diagramme de Hasse au format Mermaid dans un fichier.
 */
int export_mermaid_hasse(const t_partition *part, const t_link_array *links, const char *filename);

#endif // HASSE_H
