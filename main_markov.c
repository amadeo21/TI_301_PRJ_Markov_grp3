#include <stdio.h>
#include <stdlib.h>
#include "markov.h"
#include "hasse.h"

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <fichier_graphe>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *filename = argv[1];
    t_graph *g = read_graph(filename);
    if (!g) {
        fprintf(stderr, "Impossible de lire le graphe.\n");
        return EXIT_FAILURE;
    }

    printf("=== PARTIE 1 : GRAPHE ===\n");
    print_adj_list(g);
    check_markov_graph(g, 0.01f);
    export_mermaid_graph(g, "graph_mermaid.md");
    printf("Fichier Mermaid du graphe : graph_mermaid.md\n\n");

    printf("=== PARTIE 2 : TARJAN / PARTITION ===\n");
    t_partition part = tarjan_partition(g);
    print_partition(&part);

    t_link_array links;
    init_link_array(&links);
    build_class_links(g, &part, &links);
    removeTransitiveLinks(&links);
    export_mermaid_hasse(&part, &links, "hasse_mermaid.md");
    printf("Fichier Mermaid du diagramme de Hasse : hasse_mermaid.md\n\n");

    printf("=== PARTIE 3 : MATRICES / DISTRIBUTIONS ===\n");
    t_matrix M = matrix_from_graph(g);

    // Exemple : calcul de M^3
    t_matrix M3 = matrix_power(&M, 3);
    printf("M^3 calcule (non affiche, mais disponible pour tests).\n");

    // distribution stationnaire sur le graphe complet (si irreductible + apériodique)
    float *dist = stationary_distribution(&M, 0.01f, 50);
    printf("Distribution stationnaire approx. (ligne 1 de M^k):\n");
    for (int i = 0; i < M.cols; ++i) {
        printf("pi[%d] = %.4f\n", i + 1, dist[i]);
    }
    free(dist);

    // périodicité de la première classe (si elle existe)
    if (part.size > 0) {
        t_matrix sub = subMatrix(M, part, 0);
        int period = getPeriod(sub);
        printf("Periode de la classe %s : %d\n", part.classes[0].name, period);
        free_matrix(&sub);
    }

    free_matrix(&M);
    free_matrix(&M3);
    free_link_array(&links);
    free_partition(&part);
    free_graph(g);

    return EXIT_SUCCESS;
}