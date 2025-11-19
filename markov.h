#ifndef MARKOV_H
#define MARKOV_H

#include <stdio.h>

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
} t_adj_node;

/**
 * @brief Graphe orienté de Markov, stocké en liste d'adjacence.
 */
typedef struct {
    int nb_vertices;        /**< Nombre de sommets. */
    t_adj_node *array;      /**< Tableau de listes (taille nb_vertices). */
} t_graph;

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
    int *vertices;  /**< Tableau dynamique de sommets (1..n). */
    int size;       /**< Nombre de sommets dans la classe. */
    int capacity;   /**< Capacité du tableau. */
} t_class;

/**
 * @brief Partition du graphe en classes.
 */
typedef struct {
    t_class *classes;   /**< Tableau de classes. */
    int size;           /**< Nombre de classes. */
    int capacity;       /**< Capacité du tableau. */
} t_partition;

/**
 * @brief Matrice de float (utilisée pour les probabilités).
 */
typedef struct {
    int rows;       /**< Nombre de lignes. */
    int cols;       /**< Nombre de colonnes. */
    float **data;   /**< Tableau 2D [rows][cols]. */
} t_matrix;

/* ===================== PARTIE 1 : GRAPHES / MERMAID ===================== */

/**
 * @brief Lit un graphe à partir d'un fichier texte au format décrit dans l'énoncé.
 * @return Graphe alloué dynamiquement (à libérer avec free_graph).
 */
t_graph *read_graph(const char *filename);

/**
 * @brief Libère toute la mémoire associée au graphe.
 */
void free_graph(t_graph *g);

/**
 * @brief Affiche la liste d'adjacence sur la sortie standard (debug).
 */
void print_adj_list(const t_graph *g);

/**
 * @brief Vérifie si le graphe est un graphe de Markov.
 *        Pour chaque sommet, la somme des probabilités sortantes doit être dans [1-eps, 1+eps].
 * @param eps Tolérance (par ex. 0.01).
 * @return 1 si OK, 0 sinon (avec messages d'erreur affichés).
 */
int check_markov_graph(const t_graph *g, float eps);

/**
 * @brief Exporte le graphe au format Mermaid dans un fichier.
 */
int export_mermaid_graph(const t_graph *g, const char *filename);

/* ===================== PARTIE 2 : TARJAN / PARTITION ==================== */

/**
 * @brief Calcule la partition du graphe en composantes fortement connexes (algorithme de Tarjan).
 */
t_partition tarjan_partition(const t_graph *g);

/**
 * @brief Libère toute la mémoire d'une partition.
 */
void free_partition(t_partition *p);

/**
 * @brief Affiche la partition obtenue (format texte simple).
 */
void print_partition(const t_partition *p);

/**
 * @brief Construit un tableau qui donne, pour chaque sommet, l'indice de la classe à laquelle il appartient.
 * @return Tableau de taille nb_vertices (indice 0 -> sommet 1, etc.), à libérer par l'appelant.
 */
int *build_vertex_to_class(const t_partition *p, int nb_vertices);

/* ===================== PARTIE 3 : MATRICES / DISTRIBUTIONS ============= */

/**
 * @brief Crée la matrice de transition à partir d'un graphe (n x n).
 */
t_matrix matrix_from_graph(const t_graph *g);

/**
 * @brief Crée une matrice n x n initialisée à 0.
 */
t_matrix create_empty_matrix(int n);

/**
 * @brief Libère la mémoire d'une matrice.
 */
void free_matrix(t_matrix *m);

/**
 * @brief Recopie src dans dst (les tailles doivent être identiques).
 */
void copy_matrix(t_matrix *dst, const t_matrix *src);

/**
 * @brief Calcule C = A * B (matrices n x n).
 */
void multiply_matrices(const t_matrix *A, const t_matrix *B, t_matrix *C);

/**
 * @brief Calcule la "norme de différence" diff(M,N) = somme des |m_ij - n_ij|.
 */
float diff_matrices(const t_matrix *M, const t_matrix *N);

/**
 * @brief Extrait la sous-matrice correspondant à une classe donnée.
 * @param matrix Matrice complète.
 * @param part   Partition (classes).
 * @param compo_index Indice de la classe (0..part->size-1).
 */
t_matrix subMatrix(t_matrix matrix, t_partition part, int compo_index);

/**
 * @brief Calcule M^k (puissance k) pour une matrice n x n.
 *        Utilise des multiplications successives simples.
 */
t_matrix matrix_power(const t_matrix *M, int power);

/**
 * @brief Calcule une approximation d'une distribution stationnaire pour une matrice de transition.
 *        Méthode : puissance de matrice jusqu'à ce que diff <= eps.
 * @param M Matrice de transition d'une classe.
 * @param eps Tolérance (par ex. 0.01).
 * @param max_iter Nombre max d'itérations.
 * @return Vecteur de taille M->cols (alloué dynamiquement).
 */
float *stationary_distribution(const t_matrix *M, float eps, int max_iter);

/**
 * @brief Calcule la période d'une classe (matrice de transition de la sous-chaîne),
 *        en utilisant le code donné dans l'énoncé.
 */
int getPeriod(t_matrix sub_matrix);

#endif // MARKOV_H