#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "adjlist.h"
#include "tarjan.h"
#include "hasse.h"
#include "matrix.h"

/*
 Program for PARTIE 3:
 - builds matrix M from adjacency list
 - demonstrates M^k calculation (for k provided)
 - evolves a distribution Pi through time: Pi_{t+1} = Pi_t * M
 - attempts to find a stationary distribution Pi* (stopping when L1 diff < eps)
 - extracts submatrices for persistent classes and computes their stationary distributions
 - computes period for each class (using getPeriod)
 Usage:
   ./part3 input_graph.txt k eps:max_iter
   where:
     k = integer >=1 : compute M^k and display (optional; use 1 to skip big power)
     eps_max_iter = "<eps>:<max_iter>" e.g. "0.01:1000"
 Example:
   ./part3 exemple1.txt 3 0.001:1000
*/

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s input_graph.txt k eps:max_iter\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *input = argv[1];
    int k = atoi(argv[2]);
    char *tmp = strdup(argv[3]);
    char *p = strchr(tmp, ':');
    if (!p) {
        fprintf(stderr, "eps:max_iter argument must be of form 0.01:1000\n");
        free(tmp);
        return EXIT_FAILURE;
    }
    *p = '\0';
    float eps = atof(tmp);
    int max_iter = atoi(p+1);
    free(tmp);

    AdjList g = readGraph(input);
    printf("Lecture du graphe (%d sommets) terminée.\n\n", g.n);

    /* Matrix M */
    t_matrix M = adjlist_to_matrix(&g);
    printf("Matrice de transition M (%d x %d) :\n", M.rows, M.cols);
    print_matrix(&M, stdout);
    printf("\n");

    /* Compute M^k */
    if (k >= 1) {
        printf("Calcul de M^%d ...\n", k);
        t_matrix Mk = matrix_power(&M, k);
        printf("M^%d :\n", k);
        print_matrix(&Mk, stdout);
        free_matrix(&Mk);
        printf("\n");
    }

    /* Demonstrate distribution evolution: pick an initial distribution Pi0 = basis vector for state 1 (or ask? use state 1) */
    int n = M.rows;
    float *Pi = (float*)calloc(n, sizeof(float));
    Pi[0] = 1.0f; /* initial state 1 */
    float *Pi_next = (float*)calloc(n, sizeof(float));
    printf("Évolution d'une distribution initiale Pi0 = (1,0,0,...)\n");
    printf("Itérations jusqu'à convergence eps = %.6f (max %d itérations)\n", eps, max_iter);
    int iter = 0;
    float diff = 1.0f;
    while (iter < max_iter) {
        vector_mul_matrix(Pi, &M, Pi_next);
        /* compute L1 difference */
        diff = 0.0f;
        for (int i = 0; i < n; ++i) diff += fabsf(Pi_next[i] - Pi[i]);
        iter++;
        /* copy Pi_next -> Pi */
        for (int i = 0; i < n; ++i) Pi[i] = Pi_next[i];
        if (diff < eps) break;
    }
    printf("Convergence après %d itérations, diff = %.8f\n", iter, diff);
    printf("Distribution approchée : ");
    print_vector(Pi, n, stdout);
    printf("\n");

    /* Tarjan partition to get classes */
    t_partition_result pres = tarjan_partition(&g);
    t_partition *part = &pres.partition;
    printf("Partition (%d classes):\n", part->count);
    for (int ci = 0; ci < part->count; ++ci) {
        t_class *c = &part->classes[ci];
        printf("%s: {", c->name);
        for (int j = 0; j < c->size; ++j) {
            printf("%d", c->vertices[j]);
            if (j+1 < c->size) printf(", ");
        }
        printf("}\n");
    }
    printf("\n");

    /* Hasse links to know which classes are persistent (outdeg==0) */
    t_link_array la = build_class_links(&g, &pres);
    if (1) { /* optionally remove transitive links to get pure Hasse */
        remove_transitive_links(&la, part->count);
    }

    /* compute outdeg per class */
    int kclasses = part->count;
    int *outdeg = (int*)calloc(kclasses+1, sizeof(int));
    for (int e = 0; e < la.count; ++e) {
        int a = la.links[e].from;
        if (a >=1 && a <= kclasses) outdeg[a]++;
    }

    /* For each class that is persistent (outdeg==0), compute stationary distribution on its submatrix */
    printf("Calcul des distributions stationnaires par classes persistantes (si existantes) et périodes :\n");
    for (int ci = 0; ci < kclasses; ++ci) {
        int idx = ci + 1;
        t_class *c = &part->classes[ci];
        int is_persistent = (outdeg[idx] == 0);
        if (!is_persistent) continue;
        printf("\n%s est PERSISTANTE. Sommets: {", c->name);
        for (int j = 0; j < c->size; ++j) {
            printf("%d", c->vertices[j]);
            if (j+1 < c->size) printf(", ");
        }
        printf("}\n");

        /* extract submatrix */
        t_matrix S = subMatrix(M, *part, idx);
        printf("Sous-matrice S (classe %s) :\n", c->name);
        print_matrix(&S, stdout);

        /* Compute stationarity by iterating a distribution over S */
        int m = S.rows;
        float *pi_s = (float*)calloc(m, sizeof(float));
        float *pi_s_next = (float*)calloc(m, sizeof(float));
        /* choose initial pi_s uniform or basis (here uniform) */
        for (int i = 0; i < m; ++i) pi_s[i] = 1.0f / (float)m;

        int it_cnt = 0;
        float diff_s = 1.0f;
        while (it_cnt < max_iter) {
            vector_mul_matrix(pi_s, &S, pi_s_next); /* pi_next = pi * S */
            diff_s = 0.0f;
            for (int i = 0; i < m; ++i) diff_s += fabsf(pi_s_next[i] - pi_s[i]);
            it_cnt++;
            for (int i = 0; i < m; ++i) pi_s[i] = pi_s_next[i];
            if (diff_s < eps) break;
        }
        printf("Stationnarité (classe %s) après %d itérations, diff=%.8f : ", c->name, it_cnt, diff_s);
        print_vector(pi_s, m, stdout);

        /* Expand to full vector of size n */
        float *pi_full = (float*)calloc(n, sizeof(float));
        for (int i = 0; i < m; ++i) {
            int v = c->vertices[i] - 1;
            pi_full[v] = pi_s[i];
        }
        printf("Stationnaire étendue sur tout le graphe : ");
        print_vector(pi_full, n, stdout);

        /* compute period of this class */
        int period = getPeriod(S);
        if (period == 0) {
            printf("Période (classe %s): 0 (aucune diagonale positive détectée dans puissances <= m)\n", c->name);
        } else {
            printf("Période (classe %s) = %d\n", c->name, period);
        }

        free(pi_s); free(pi_s_next); free(pi_full);
        free_matrix(&S);
    }

    /* For non-irreducible graphs we can also report which classes are transient */
    printf("\nPropriétés globales :\n");
    for (int ci = 0; ci < kclasses; ++ci) {
        int idx = ci+1;
        t_class *c = &part->classes[ci];
        int persistent = (outdeg[idx] == 0);
        int absorbant = (persistent && c->size == 1);
        printf("%s: %s%s\n", c->name, persistent ? "PERSISTANTE" : "TRANSITOIRE", absorbant ? " (ABSORBANTE)" : "");
    }

    /* Free resources */
    free(outdeg);
    free_link_array(&la);
    free_partition_result(&pres);
    free_matrix(&M);
    free(Pi); free(Pi_next);

    return EXIT_SUCCESS;
}
