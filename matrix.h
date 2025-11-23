#ifndef MATRIX_H
#define MATRIX_H

#include "tarjan.h"
#include "adjlist.h"

/* Matrice en stockage contigu (rows x cols), indexée 0..rows-1, 0..cols-1 */
typedef struct {
    int rows;
    int cols;
    float *data; /* length rows * cols, row-major */
} t_matrix;

/* Allocation / libération */
t_matrix create_matrix(int rows, int cols);    /* crée matrice initialisée à 0 */
void free_matrix(t_matrix *m);
t_matrix copy_matrix(const t_matrix *src);     /* copie profonde */

/* Accès */
static inline float matrix_get(const t_matrix *m, int i, int j) {
    return m->data[i * m->cols + j];
}
static inline void matrix_set(t_matrix *m, int i, int j, float v) {
    m->data[i * m->cols + j] = v;
}

/* Opérations */
void multiply_matrices(const t_matrix *A, const t_matrix *B, t_matrix *out); /* out must be sized appropriately */
t_matrix matrix_power(const t_matrix *M, int power); /* naive repeated multiply; power>=1 */
float matrix_diff(const t_matrix *A, const t_matrix *B); /* sum absolute diffs of same size */

/* Conversion depuis/vers adjacency list */
t_matrix adjlist_to_matrix(const AdjList *g);

/* Vecteur (ligne) operations */
void vector_mul_matrix(const float *vec_in, const t_matrix *M, float *vec_out); /* vec_in size = M->rows, vec_out size = M->cols (should be same for square) */

/* Submatrix extraction by class (partition) */
t_matrix subMatrix(const t_matrix matrix, const t_partition part, int compo_index); /* compo_index 1..part.count */

/* Period / gcd helper (défi) */
int gcd_int_array(int *vals, int nbvals);
int getPeriod(const t_matrix sub_matrix); /* retourne période (0 si indéterminé ou aucun diag trouvé) */

/* Utilities */
void print_matrix(const t_matrix *m, FILE *out);
void print_vector(const float *v, int n, FILE *out);

#endif /* MATRIX_H */
