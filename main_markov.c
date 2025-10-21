#include <stdio.h>

// main_markov.c — exécutable "markov" (lecture, affichage, vérification, export Mermaid)
#include "markov.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(const char* prog) {
    fprintf(stderr,
        "Usage : %s <fichier_graphe.txt> [--print] [--verify] [--mermaid <out.mmd>]\n"
        "Exemple: %s data/exemple1.txt --print --verify --mermaid graphe.mmd\n",
        prog, prog);
}

int main(int argc, char** argv) {
    if (argc < 2) { usage(argv[0]); return EXIT_FAILURE; }

    const char* infile = argv[1];
    int i = 2;
    int do_print = 0, do_verify = 0, do_mmd = 0;
    const char* mmd_out = NULL;

    while (i < argc) {
        if (strcmp(argv[i], "--print") == 0) { do_print = 1; i++; }
        else if (strcmp(argv[i], "--verify") == 0) { do_verify = 1; i++; }
        else if (strcmp(argv[i], "--mermaid") == 0 && i+1 < argc) { do_mmd = 1; mmd_out = argv[i+1]; i += 2; }
        else { usage(argv[0]); return EXIT_FAILURE; }
    }

    AdjList G = readGraph(infile);

    if (do_print) adj_print(&G);
    if (do_verify) verify_markov(&G, 0.99f, 1.01f);
    if (do_mmd) {
        if (!mmd_out) { fprintf(stderr, "--mermaid nécessite un chemin de sortie\n"); adj_free(&G); return EXIT_FAILURE; }
        if (write_mermaid(&G, mmd_out)) printf("Fichier Mermaid écrit : %s\n", mmd_out);
        else fprintf(stderr, "Échec d'écriture du fichier Mermaid.\n");
    }

    adj_free(&G);
    return EXIT_SUCCESS;
}