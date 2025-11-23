#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"
#include "tarjan.h"
#include "hasse.h"
#include "matrix.h"

int main(int argc, char **argv)
{
    const char *filename = argv[1];
    t_graph *g = readGraph(filename);

    printf("=== PARTIE 1 : GRAPHE / MARKOV / MERMAID ===\n");
    printAdjList(g);
    checkMarkov(g, 0.01f);
    exportMermaidGraph(g, "graph_mermaid.mmd");
    printf("Fichier Mermaid du graphe : graph_mermaid.mmd\n");

    printf("\n=== PARTIE 2 : TARJAN / PARTITION / HASSE ===\n");
    t_partition part = tarjanPartition(g);
    printPartition(&part);

    t_link_array links;
    init_link_array(&links);
    build_class_links(g, &part, &links);
    removeTransitiveLinks(&links);
    export_mermaid_hasse(&part, &links, "hasse_mermaid.mmd");
    printf("Fichier Mermaid du diagramme de Hasse : hasse_mermaid.mmd\n");

    classify_graph(g, &part, &links);

    printf("\n=== PARTIE 3 : MATRICES / DISTRIBUTIONS / PERIODE ===\n");

    t_matrix M = createMatrixFromGraph(g);
    printMatrix(&M, "M");

    t_matrix M3 = matrixPower(&M, 3);
    printMatrix(&M3, "M^3");

    t_matrix M7 = matrixPower(&M, 7);
    printMatrix(&M7, "M^7");

    int power_limit = 0;
    t_matrix Mlim = iterateUntilStationary(&M, 0.01f, 50, &power_limit);
    printf("\nPuissance n telle que diff(M^n, M^(n-1)) < 0.01 : n = %d\n", power_limit);
    printMatrix(&Mlim, "M^n (limite approx)");

    printf("\nDistributions stationnaires par classe (approx) :\n");
    for (int ci = 0; ci < part.size; ++ci) {
        t_matrix sub = subMatrix(M, part, ci);
        if (sub.rows == 0) continue;
        int kclass = 0;
        t_matrix sub_lim = iterateUntilStationary(&sub, 0.01f, 50, &kclass);
        printf("\nClasse %s (puissance n=%d): distribution stationnaire approx (ligne 1):\n",
               part.classes[ci].name, kclass);
        for (int j = 0; j < sub_lim.cols; ++j) {
            printf("  p[%d] = %.4f\n", j + 1, sub_lim.data[0][j]);
        }
        freeMatrix(&sub_lim);
        freeMatrix(&sub);
    }

    printf("\nPeriode de chaque classe :\n");
    for (int ci = 0; ci < part.size; ++ci) {
        t_matrix sub = subMatrix(M, part, ci);
        if (sub.rows == 0) continue;
        int period = getPeriod(sub);
        printf("  Classe %s : periode = %d\n", part.classes[ci].name, period);
        freeMatrix(&sub);
    }

    freeMatrix(&M);
    freeMatrix(&M3);
    freeMatrix(&M7);
    freeMatrix(&Mlim);
    free_link_array(&links);
    freePartition(&part);
    freeGraph(g);

    return EXIT_SUCCESS;
}
