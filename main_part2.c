#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "adjlist.h"
#include "tarjan.h"
#include "hasse.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s input_graph.txt output_hasse_mermaid.txt [--remove-transitives]\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *input = argv[1];
    const char *out_mermaid = argv[2];
    int remove_transitives_flag = 0;
    if (argc >= 4 && strcmp(argv[3], "--remove-transitives") == 0) remove_transitives_flag = 1;

    AdjList g = readGraph(input);
    printf("Lecture du graphe (%d sommets) terminée.\n\n", g.n);

    /* Tarjan -> partition */
    t_partition_result pres = tarjan_partition(&g);
    t_partition *part = &pres.partition;

    printf("Partition (composantes fortement connexes) trouvée :\n");
    for (int i = 0; i < part->count; ++i) {
        t_class *c = &part->classes[i];
        printf("%s: {", c->name);
        for (int j = 0; j < c->size; ++j) {
            printf("%d", c->vertices[j]);
            if (j+1 < c->size) printf(", ");
        }
        printf("}\n");
    }
    printf("\n");

    /* vertex -> class */
    printf("Table vertex -> class :\n");
    for (int v = 1; v <= g.n; ++v) {
        printf("%d -> C%d\n", v, pres.vertex_to_class[v]);
    }
    printf("\n");

    /* build class links */
    t_link_array la = build_class_links(&g, &pres);
    printf("Liens entre classes (avec possibles redondances) :\n");
    for (int e = 0; e < la.count; ++e) {
        printf("C%d -> C%d\n", la.links[e].from, la.links[e].to);
    }
    printf("\n");

    if (remove_transitives_flag) {
        remove_transitive_links(&la, part->count);
        printf("Liens après suppression des redondances (Hasse) :\n");
        for (int e = 0; e < la.count; ++e) {
            printf("C%d -> C%d\n", la.links[e].from, la.links[e].to);
        }
        printf("\n");
    } else {
        printf("Remarque : pour obtenir le vrai diagramme de Hasse sans redondances, relancer avec --remove-transitives\n\n");
    }

    /* properties */
    print_class_properties(&g, &pres, &la);

    /* write mermaid */
    if (write_hasse_mermaid(part, &la, out_mermaid) == 0) {
        printf("\nFichier Mermaid (Hasse) généré : %s\n", out_mermaid);
        printf("Collez son contenu sur https://www.mermaidchart.com/ pour visualiser le diagramme de Hasse.\n");
    } else {
        fprintf(stderr, "Erreur lors de la génération du fichier mermaid Hasse.\n");
    }

    /* cleanup */
    free_link_array(&la);
    free_partition_result(&pres);
    free_adjlist(&g);

    return EXIT_SUCCESS;
}
