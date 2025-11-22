#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ----------------------------------------------------------- */
/*                 OUTILS DE SECURISATION                      */
/* ----------------------------------------------------------- */

/* lit une ligne entière de manière sûre */
void input_line(char *buffer, int size) {
    fgets(buffer, size, stdin);
    /* supprime le \n final */
    buffer[strcspn(buffer, "\n")] = 0;
}

/* lit un entier valide : boucle tant que l’entrée n’est pas correcte */
int input_int(const char *msg) {
    char buffer[256];
    int value;

    while (1) {
        printf("%s", msg);
        input_line(buffer, sizeof(buffer));

        /* sscanf retourne 1 si un entier a été trouvé */
        if (sscanf(buffer, "%d", &value) == 1) {
            return value;
        }
        printf("Erreur : veuillez entrer un entier valide.\n");
    }
}

/* lit un couple (u v) valide */
void input_arc(const char *msg, int *u, int *v, int maxSommet) {
    char buffer[256];

    while (1) {
        printf("%s", msg);
        input_line(buffer, sizeof(buffer));

        if (sscanf(buffer, "%d %d", u, v) == 2) {
            if (*u >= 0 && *u < maxSommet && *v >= 0 && *v < maxSommet) {
                return;
            }
            printf("Arcs hors bornes : valeurs autorisées = 0 à %d\n",
                   maxSommet - 1);
        } else {
            printf("Format invalide. Exemple attendu : 0 1\n");
        }
    }
}

/* vérifie format eps:max_iter */
int validate_eps_max(const char *str) {
    int i, colon = 0;

    for (i = 0; str[i]; i++) {
        if (str[i] == ':') {
            colon = 1;
        } else if (!isdigit(str[i]) && str[i] != '.') {
            return 0;
        }
    }
    return colon;
}

/* ----------------------------------------------------------- */
/*     GENERATION DU FICHIER INPUT A PARTIR DES ENTREES        */
/* ----------------------------------------------------------- */

void generate_input_file(const char *filename) {
    int n = input_int("\n--- GENERATION DU GRAPHE ---\nNombre de sommets : ");
    int m = input_int("Nombre d'arcs : ");

    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("Impossible d’écrire dans %s\n", filename);
        exit(1);
    }

    fprintf(f, "%d %d\n", n, m);

    for (int i = 0; i < m; i++) {
        int u, v;
        char msg[128];
        sprintf(msg, "Arc %d (u v) : ", i + 1);
        input_arc(msg, &u, &v, n);
        fprintf(f, "%d %d\n", u, v);
    }

    fclose(f);
    printf("✔ Fichier '%s' créé avec succès.\n", filename);
}

/* ----------------------------------------------------------- */
/*            FONCTIONS D’APPEL VERS PART1/2/3                 */
/* ----------------------------------------------------------- */

int run_part1() {
    char input[] = "input_generated.txt";
    char output[256];
    char command[512];

    generate_input_file(input);

    printf("Nom du fichier de sortie Mermaid : ");
    input_line(output, sizeof(output));

#ifdef _WIN32
    sprintf(command, "part1.exe %s %s", input, output);
#else
    sprintf(command, "./part1 %s %s", input, output);
#endif

    system(command);
    return 0;
}

int run_part2() {
    char input[] = "input_generated.txt";
    char output[256];
    char removeFlag[32];
    char command[512];

    generate_input_file(input);

    printf("Nom du fichier de sortie Mermaid : ");
    input_line(output, sizeof(output));

    printf("Supprimer les liens transitifs ? (oui/non) : ");
    input_line(removeFlag, sizeof(removeFlag));

#ifdef _WIN32
    if (strcmp(removeFlag, "oui") == 0)
        sprintf(command, "part2.exe %s %s --remove-transitives", input, output);
    else
        sprintf(command, "part2.exe %s %s", input, output);
#else
    if (strcmp(removeFlag, "oui") == 0)
        sprintf(command, "./part2 %s %s --remove-transitives", input, output);
    else
        sprintf(command, "./part2 %s %s", input, output);
#endif

    system(command);
    return 0;
}

int run_part3() {
    char input[] = "input_generated.txt";
    char epsmax[256];
    char command[512];

    generate_input_file(input);

    int k = input_int("k (ex: 2 pour M^2) : ");

    while (1) {
        printf("eps:max_iter (ex: 0.001:1000) : ");
        input_line(epsmax, sizeof(epsmax));

        if (validate_eps_max(epsmax)) break;
        printf("Format invalide. Exemple correct : 0.001:1000\n");
    }

#ifdef _WIN32
    sprintf(command, "part3.exe %s %d %s", input, k, epsmax);
#else
    sprintf(command, "./part3 %s %d %s", input, k, epsmax);
#endif

    system(command);
    return 0;
}

/* ----------------------------------------------------------- */
/*                         MENU                                */
/* ----------------------------------------------------------- */

int main() {
    printf("=== MENU DU PROJET MARKOV ===\n");
    printf("1 - Partie 1 (lecture + Mermaid)\n");
    printf("2 - Partie 2 (Tarjan + Hasse)\n");
    printf("3 - Partie 3 (Matrices + stationnarité)\n");

    int choix = input_int("Votre choix : ");

    if (choix == 1) run_part1();
    else if (choix == 2) run_part2();
    else if (choix == 3) run_part3();
    else printf("Choix invalide.\n");

    return 0;
}