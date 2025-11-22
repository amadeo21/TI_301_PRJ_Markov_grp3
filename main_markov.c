// =====================================================
// main_markov.c
// Exécutable "markov"
// - Lecture du fichier de graphe
// - Affichage de la liste d’adjacence
// - Vérification des probabilités (graphe de Markov)
// - Export Mermaid pour visualisation
// =====================================================

#include "markov.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------
// Fonction d’aide : affichage de la syntaxe d’utilisation
// -----------------------------------------------------
static void usage(const char* prog) {
    fprintf(stderr,
        "Usage : %s <fichier_graphe.txt> [--print] [--verify] [--mermaid <out.mmd>]\n"
        "Exemple: %s data/exemple1.txt --print --verify --mermaid graphe.mmd\n",
        prog, prog);
}

// -----------------------------------------------------
// Fonction principale
// -----------------------------------------------------
int main(int argc, char** argv) {

    // Vérifie qu’un fichier a bien été passé en argument
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char* infile = argv[1];
    int i = 2;

    // Flags pour les options
    int do_print = 0;
    int do_verify = 0;
    int do_mmd = 0;

    const char* mmd_out = NULL;

    // Lecture des arguments optionnels
    while (i < argc) {
        if (strcmp(argv[i], "--print") == 0) {
            do_print = 1;
            i++;
        }
        else if (strcmp(argv[i], "--verify") == 0) {
            do_verify = 1;
            i++;
        }
        else if (strcmp(argv[i], "--mermaid") == 0 && i + 1 < argc) {
            do_mmd = 1;
            mmd_out = argv[i + 1];
            i += 2;
        }
        else {
            usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    // -------------------------------------------------
    // 1. Lecture du graphe depuis le fichier texte
    // -------------------------------------------------
    AdjList G = readGraph(infile);

    // -------------------------------------------------
    // 2. Affichage de la liste d’adjacence (optionnel)
    // -------------------------------------------------
    if (do_print)
        adj_print(&G);

    // -------------------------------------------------
    // 3. Vérification du graphe de Markov (optionnel)
    // -------------------------------------------------
    if (do_verify)
        verify_markov(&G, 0.99f, 1.01f);

    // -------------------------------------------------
    // 4. Export au format Mermaid (optionnel)
    // -------------------------------------------------
    if (do_mmd) {
        if (!mmd_out) {
            fprintf(stderr, "--mermaid nécessite un chemin de sortie\n");
            adj_free(&G);
            return EXIT_FAILURE;
        }
        if (write_mermaid(&G, mmd_out))
            printf("Fichier Mermaid écrit : %s\n", mmd_out);
        else
            fprintf(stderr, "Échec d'écriture du fichier Mermaid.\n");
    }

    // -------------------------------------------------
    // 5. Libération de la mémoire
    // -------------------------------------------------
    adj_free(&G);

    return EXIT_SUCCESS;
}