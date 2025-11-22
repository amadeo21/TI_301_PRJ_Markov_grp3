#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "adjlist.h"

/* Programme de test / utilitaire pour la partie 1.
   Usage:
     ./part1 exemple1.txt output_mermaid.txt

   Il lit le graphe, affiche la liste d'adjacence, vérifie Markov et génère le fichier mermaid.
*/

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s input_graph.txt output_mermaid.txt\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *input = argv[1];
    const char *out_mermaid = argv[2];

    AdjList g = readGraph(input);
    printf("Lecture du graphe (%d sommets) terminée.\n\n", g.n);

    printf("Affichage de la liste d'adjacence :\n");
    print_adjlist(&g, stdout);
    printf("\n");

    /* Vérification Markov : conformément au sujet,
       la somme des probabilités sortantes devra être comprise entre 0.99 et 1.0.
       On accepte une petite marge numérique d'un millième au dessus (1.001) pour éviter
       des rejets liés aux arrondis d'écriture. */
    float lower = 0.99f;
    float upper = 1.001f;
    int is_markov = verify_markov(&g, lower, upper);
    (void)is_markov;

    /* Génération Mermaid */
    if (write_mermaid(&g, out_mermaid) == 0) {
        printf("Fichier Mermaid généré : %s\n", out_mermaid);
        printf("Collez son contenu sur https://www.mermaidchart.com/ pour visualiser le graphe.\n");
    } else {
        fprintf(stderr, "Erreur lors de la génération du fichier mermaid.\n");
    }

    free_adjlist(&g);
    return EXIT_SUCCESS;
}
