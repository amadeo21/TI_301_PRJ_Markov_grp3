#ifndef GRAPH_H
#define GRAPH_H

/**
 * @brief Arc de la liste d'adjacence (une arête du graphe).
 */
typedef struct s_arc {
    int dest;               /**< Sommet d'arrivée (1..n). */
    float proba;            /**< Probabilité associée. */
    struct s_arc *next;     /**< Pointeur vers l'arc suivant. */
} t_arc;

/**
 * @brief Liste d'adjacence pour un sommet.
 */
typedef struct {
    t_arc *head;            /**< Tête de la liste d'arcs sortants. */
} t_adj_list;

/**
 * @brief Graphe orienté de Markov, stocké en liste d'adjacence.
 */
typedef struct {
    int nb_vertices;        /**< Nombre de sommets. */
    t_adj_list *array;      /**< Tableau de listes (taille nb_vertices). */
} t_graph;

/**
 * @brief Lit un graphe à partir d'un fichier texte au format du sujet.
 */
t_graph *readGraph(const char *filename);

/**
 * @brief Libère toute la mémoire associée au graphe.
 */
void freeGraph(t_graph *g);

/**
 * @brief Affiche la liste d'adjacence (debug).
 */
void printAdjList(const t_graph *g);

/**
 * @brief Vérifie si le graphe est un graphe de Markov (sommes des probas ~ 1).
 */
int checkMarkov(const t_graph *g, float eps);

/**
 * @brief Exporte le graphe au format Mermaid dans un fichier .mmd.
 */
int exportMermaidGraph(const t_graph *g, const char *filename);

#endif // GRAPH_H
