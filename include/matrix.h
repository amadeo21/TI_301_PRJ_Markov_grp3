#ifndef MATRIX_H
#define MATRIX_H

#include "graph.h"
#include "tarjan.h"

/**
 * @brief Matrice de float (utilisée pour les probabilités).
 */
typedef struct {
    int rows;
    int cols;
    float **data;
} t_matrix;

/**
 * @brief Crée une matrice n x n initialisée à 0.
 */
t_matrix createEmptyMatrix(int n);

/**
 * @brief Libère la mémoire d'une matrice.
 */
void freeMatrix(t_matrix *m);

/**
 * @brief Crée la matrice de transition à partir d'un graphe (n x n).
 */
t_matrix createMatrixFromGraph(const t_graph *g);

/**
 * @brief Recopie src dans dst (les tailles doivent être identiques).
 */
void copyMatrix(t_matrix *dst, const t_matrix *src);

/**
 * @brief Calcule C = A * B (matrices n x n).
 */
void multiplyMatrices(const t_matrix *A, const t_matrix *B, t_matrix *C);

/**
 * @brief Calcule la "norme de différence" diff(M,N) = somme des |m_ij - n_ij|.
 */
float diffMatrices(const t_matrix *M, const t_matrix *N);

/**
 * @brief Affiche une matrice avec un nom.
 */
void printMatrix(const t_matrix *M, const char *name);

/**
 * @brief Calcule M^power (puissance entière).
 */
t_matrix matrixPower(const t_matrix *M, int power);

/**
 * @brief Extrait la sous-matrice correspondant à une classe donnée.
 */
t_matrix subMatrix(t_matrix matrix, t_partition part, int compo_index);

/**
 * @brief Itère les puissances de M jusqu'à ce que diff(M^n, M^(n-1)) < eps
 * ou max_iter soit atteint. Renvoie M^n, place n dans *power_out.
 */
t_matrix iterateUntilStationary(const t_matrix *M, float eps, int max_iter, int *power_out);

/**
 * @brief Calcule la période d'une classe (matrice de transition de la sous-chaîne).
 */
int getPeriod(t_matrix sub_matrix);

#endif // MATRIX_H
