#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "utils.h"

static float **alloc_matrix_data(int n)
{
    float **data = malloc(sizeof(float *) * (size_t)n);
    if (!data) {
        perror("malloc matrix rows");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; ++i) {
        data[i] = calloc((size_t)n, sizeof(float));
        if (!data[i]) {
            perror("calloc matrix row");
            exit(EXIT_FAILURE);
        }
    }
    return data;
}

t_matrix createEmptyMatrix(int n)
{
    t_matrix M;
    M.rows = n;
    M.cols = n;
    M.data = alloc_matrix_data(n);
    return M;
}

void freeMatrix(t_matrix *m)
{
    if (!m || !m->data) return;
    for (int i = 0; i < m->rows; ++i) {
        free(m->data[i]);
    }
    free(m->data);
    m->data = NULL;
    m->rows = m->cols = 0;
}

t_matrix createMatrixFromGraph(const t_graph *g)
{
    int n = g->nb_vertices;
    t_matrix M = createEmptyMatrix(n);
    for (int i = 0; i < n; ++i) {
        for (t_arc *cur = g->array[i].head; cur; cur = cur->next) {
            int j = cur->dest - 1;
            M.data[i][j] = cur->proba;
        }
    }
    return M;
}

void copyMatrix(t_matrix *dst, const t_matrix *src)
{
    if (!dst || !src || dst->rows != src->rows || dst->cols != src->cols) return;
    for (int i = 0; i < src->rows; ++i) {
        for (int j = 0; j < src->cols; ++j) {
            dst->data[i][j] = src->data[i][j];
        }
    }
}

void multiplyMatrices(const t_matrix *A, const t_matrix *B, t_matrix *C)
{
    int n = A->rows;
    if (A->cols != n || B->rows != n || B->cols != n ||
        C->rows != n || C->cols != n) {
        fprintf(stderr, "Erreur dimensions multiplication matrices\n");
        return;
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < n; ++k) {
                sum += A->data[i][k] * B->data[k][j];
            }
            C->data[i][j] = sum;
        }
    }
}

float diffMatrices(const t_matrix *M, const t_matrix *N)
{
    if (!M || !N || M->rows != N->rows || M->cols != N->cols) {
        fprintf(stderr, "diffMatrices: dimensions incompatibles\n");
        return 0.0f;
    }
    float sum = 0.0f;
    for (int i = 0; i < M->rows; ++i) {
        for (int j = 0; j < M->cols; ++j) {
            sum += fabsf(M->data[i][j] - N->data[i][j]);
        }
    }
    return sum;
}

void printMatrix(const t_matrix *M, const char *name)
{
    if (!M) return;
    printf("\nMatrice %s (%d x %d):\n", name, M->rows, M->cols);
    for (int i = 0; i < M->rows; ++i) {
        for (int j = 0; j < M->cols; ++j) {
            printf("%7.4f ", M->data[i][j]);
        }
        printf("\n");
    }
}

t_matrix matrixPower(const t_matrix *M, int power)
{
    int n = M->rows;
    t_matrix result = createEmptyMatrix(n);
    t_matrix tmp = createEmptyMatrix(n);

    for (int i = 0; i < n; ++i) {
        result.data[i][i] = 1.0f;
    }

    t_matrix base = createEmptyMatrix(n);
    copyMatrix(&base, M);

    for (int p = 0; p < power; ++p) {
        multiplyMatrices(&result, &base, &tmp);
        copyMatrix(&result, &tmp);
    }

    freeMatrix(&base);
    freeMatrix(&tmp);
    return result;
}

t_matrix subMatrix(t_matrix matrix, t_partition part, int compo_index)
{
    if (compo_index < 0 || compo_index >= part.size) {
        fprintf(stderr, "subMatrix: indice de composante invalide\n");
        t_matrix empty = {0, 0, NULL};
        return empty;
    }

    t_class *c = &part.classes[compo_index];
    int k = c->size;
    t_matrix sub = createEmptyMatrix(k);

    for (int i = 0; i < k; ++i) {
        int vi = c->vertices[i] - 1;
        for (int j = 0; j < k; ++j) {
            int vj = c->vertices[j] - 1;
            sub.data[i][j] = matrix.data[vi][vj];
        }
    }
    return sub;
}

t_matrix iterateUntilStationary(const t_matrix *M, float eps, int max_iter, int *power_out)
{
    int n = M->rows;
    t_matrix Mk = createEmptyMatrix(n);
    t_matrix Mprev = createEmptyMatrix(n);
    copyMatrix(&Mk, M);
    copyMatrix(&Mprev, M);

    float d = 0.0f;
    int k;
    for (k = 2; k <= max_iter; ++k) {
        t_matrix next = createEmptyMatrix(n);
        multiplyMatrices(&Mk, M, &next);
        d = diffMatrices(&Mk, &next);
        freeMatrix(&Mprev);
        Mprev = Mk;
        Mk = next;
        if (d < eps) {
            break;
        }
    }
    if (power_out) *power_out = k;
    freeMatrix(&Mprev);
    return Mk;
}

/* ================= Périodicité (bonus) ================= */

static int gcd_int(int a, int b)
{
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

static int gcd_array(int *vals, int nbvals)
{
    if (nbvals == 0) return 0;
    int result = vals[0];
    for (int i = 1; i < nbvals; ++i) {
        result = gcd_int(result, vals[i]);
    }
    return result;
}

int getPeriod(t_matrix sub_matrix)
{
    int n = sub_matrix.rows;
    int *periods = malloc((size_t)n * sizeof(int));
    if (!periods) {
        perror("malloc periods");
        exit(EXIT_FAILURE);
    }
    int period_count = 0;
    int cpt;

    t_matrix power_matrix = createEmptyMatrix(n);
    t_matrix result_matrix = createEmptyMatrix(n);
    copyMatrix(&power_matrix, &sub_matrix);

    for (cpt = 1; cpt <= n; cpt++)
    {
        int diag_nonzero = 0;
        for (int i = 0; i < n; i++)
        {
            if (power_matrix.data[i][i] > 0.0f)
            {
                diag_nonzero = 1;
                break;
            }
        }
        if (diag_nonzero) {
            periods[period_count] = cpt;
            period_count++;
        }
        multiplyMatrices(&power_matrix, &sub_matrix, &result_matrix);
        copyMatrix(&power_matrix, &result_matrix);
    }

    int period = gcd_array(periods, period_count);

    free(periods);
    freeMatrix(&power_matrix);
    freeMatrix(&result_matrix);
    return period;
}
