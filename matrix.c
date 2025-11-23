#include "matrix.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/* ---------- Allocation / libération ---------- */

t_matrix create_matrix(int rows, int cols) {
    t_matrix M;
    if (rows < 0) rows = 0;
    if (cols < 0) cols = 0;
    M.rows = rows;
    M.cols = cols;
    if (rows == 0 || cols == 0) {
        M.data = NULL;
    } else {
        M.data = (float*)calloc(rows * cols, sizeof(float));
        if (!M.data) { perror("calloc matrix"); exit(EXIT_FAILURE); }
    }
    return M;
}

void free_matrix(t_matrix *m) {
    if (!m) return;
    if (m->data) free(m->data);
    m->data = NULL;
    m->rows = 0;
    m->cols = 0;
}

t_matrix copy_matrix(const t_matrix *src) {
    t_matrix dst = create_matrix(src->rows, src->cols);
    if (src->data && dst.data) {
        memcpy(dst.data, src->data, sizeof(float) * src->rows * src->cols);
    }
    return dst;
}

/* ---------- Multiplication ---------- */
/* naive triple loop; assumes dimensions compatible (A.cols == B.rows) and out sized rows=A.rows, cols=B.cols */
void multiply_matrices(const t_matrix *A, const t_matrix *B, t_matrix *out) {
    if (!A || !B || !out) return;
    if (A->cols != B->rows || out->rows != A->rows || out->cols != B->cols) {
        fprintf(stderr, "multiply_matrices: dimension mismatch\n");
        exit(EXIT_FAILURE);
    }
    int n = A->rows;
    int m = B->cols;
    int p = A->cols; /* == B->rows */
    /* zero out out */
    for (int i = 0; i < out->rows * out->cols; ++i) out->data[i] = 0.0f;
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < p; ++k) {
            float aik = matrix_get(A, i, k);
            if (aik == 0.0f) continue;
            for (int j = 0; j < m; ++j) {
                out->data[i * out->cols + j] += aik * matrix_get(B, k, j);
            }
        }
    }
}

/* ---------- Matrix power (naive repeated multiplication) ---------- */
t_matrix matrix_power(const t_matrix *M, int power) {
    if (power < 1) { fprintf(stderr, "matrix_power: power must be >=1\n"); exit(EXIT_FAILURE); }
    if (M->rows != M->cols) { fprintf(stderr, "matrix_power: matrix must be square\n"); exit(EXIT_FAILURE); }
    t_matrix result = copy_matrix(M); /* M^1 */
    t_matrix tmp = create_matrix(M->rows, M->cols);
    for (int p = 2; p <= power; ++p) {
        multiply_matrices(&result, M, &tmp); /* tmp = result * M */
        free_matrix(&result);
        result = copy_matrix(&tmp);
    }
    free_matrix(&tmp);
    return result;
}

/* ---------- difference / norm ---------- */
float matrix_diff(const t_matrix *A, const t_matrix *B) {
    if (!A || !B) return -1.0f;
    if (A->rows != B->rows || A->cols != B->cols) {
        fprintf(stderr, "matrix_diff: dimension mismatch\n");
        exit(EXIT_FAILURE);
    }
    float sum = 0.0f;
    int total = A->rows * A->cols;
    for (int i = 0; i < total; ++i) sum += fabsf(A->data[i] - B->data[i]);
    return sum;
}

/* ---------- conversion adjacency list -> matrix ---------- */
t_matrix adjlist_to_matrix(const AdjList *g) {
    int n = g->n;
    t_matrix M = create_matrix(n, n);
    for (int i = 1; i <= n; ++i) {
        Cell *cur = g->lists[i].head;
        while (cur) {
            int j = cur->arrivee;
            float p = cur->proba;
            matrix_set(&M, i-1, j-1, p); /* ligne i-1 correspond aux sorties de sommet i */
            cur = cur->next;
        }
    }
    return M;
}

/* ---------- vector (row) * matrix ---------- */
/* vec_in length = n (row), M rows = n, M cols = n (square) ; vec_out length = n */
void vector_mul_matrix(const float *vec_in, const t_matrix *M, float *vec_out) {
    int n = M->cols;
    /* initialize */
    for (int j = 0; j < n; ++j) vec_out[j] = 0.0f;
    for (int i = 0; i < M->rows; ++i) {
        float vi = vec_in[i];
        if (vi == 0.0f) continue;
        for (int j = 0; j < M->cols; ++j) {
            vec_out[j] += vi * matrix_get(M, i, j);
        }
    }
}

/* ---------- subMatrix: extrait la sous-matrice correspondant à une composante ---------- */
t_matrix subMatrix(const t_matrix matrix, const t_partition part, int compo_index) {
    if (compo_index < 1 || compo_index > part.count) {
        fprintf(stderr, "subMatrix: invalid compo_index %d\n", compo_index);
        exit(EXIT_FAILURE);
    }
    t_class *c = &part.classes[compo_index - 1];
    int m = c->size;
    t_matrix S = create_matrix(m, m);
    for (int i = 0; i < m; ++i) {
        int vi = c->vertices[i] - 1; /* convert to 0-based */
        for (int j = 0; j < m; ++j) {
            int vj = c->vertices[j] - 1;
            float val = matrix_get(&matrix, vi, vj);
            matrix_set(&S, i, j, val);
        }
    }
    return S;
}

/* ---------- printing helpers ---------- */
void print_matrix(const t_matrix *m, FILE *out) {
    if (!m || !out) return;
    for (int i = 0; i < m->rows; ++i) {
        for (int j = 0; j < m->cols; ++j) {
            fprintf(out, "%.6f ", matrix_get(m, i, j));
        }
        fprintf(out, "\n");
    }
}

void print_vector(const float *v, int n, FILE *out) {
    fprintf(out, "(");
    for (int i = 0; i < n; ++i) {
        fprintf(out, "%.6f", v[i]);
        if (i+1 < n) fprintf(out, " ");
    }
    fprintf(out, ")\n");
}

/* ---------- gcd helper and period calculation (adapted from sujet) ---------- */
int gcd_int_array(int *vals, int nbvals) {
    if (nbvals == 0) return 0;
    int result = vals[0];
    for (int i = 1; i < nbvals; ++i) {
        int a = result;
        int b = vals[i];
        /* gcd iterative */
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        result = a;
    }
    return result;
}

/* getPeriod: determine period for class represented by sub_matrix
   Algorithm: for cpt = 1..n check if diagonal of M^cpt has any positive entry.
   collect cpt where diag positive -> gcd of these cpt is period.
*/
int getPeriod(const t_matrix sub_matrix) {
    int n = sub_matrix.rows;
    if (n == 0) return 0;
    int *periods = (int*)malloc(n * sizeof(int));
    int period_count = 0;
    int cpt = 1;

    /* power_matrix starts as sub_matrix (M^1) */
    t_matrix power_matrix = copy_matrix(&sub_matrix);
    t_matrix result_matrix = create_matrix(n, n);

    for (cpt = 1; cpt <= n; ++cpt) {
        int diag_nonzero = 0;
        for (int i = 0; i < n; ++i) {
            if (matrix_get(&power_matrix, i, i) > 0.0f) {
                diag_nonzero = 1;
                break;
            }
        }
        if (diag_nonzero) {
            periods[period_count++] = cpt;
        }
        /* multiply power_matrix by sub_matrix to get next power (M^(cpt+1)) */
        multiply_matrices(&power_matrix, &sub_matrix, &result_matrix);
        free_matrix(&power_matrix);
        power_matrix = copy_matrix(&result_matrix);
    }

    free_matrix(&power_matrix);
    free_matrix(&result_matrix);

    if (period_count == 0) {
        free(periods);
        return 0;
    }
    int g = gcd_int_array(periods, period_count);
    free(periods);
    return g;
}
