# Projet Markov

Ce projet porte sur la manipulation d’un graphe orienté représentant une chaîne de Markov. Il lit un fichier décrivant les probabilités de transition entre états, vérifie que le graphe satisfait bien la propriété de Markov, identifie les classes à l’aide de l’algorithme de Tarjan, construit le diagramme de Hasse correspondant, puis analyse la matrice de transition (puissances, distribution stationnaire, période d’une classe).

---

## Fichier d’entrée

Le fichier doit commencer par le nombre total de sommets, suivi d’une ligne par transition :

depart arrivee probabilite

Exemple :

4
1 2 0.5
1 3 0.5
2 4 1.0
3 4 1.0
4 4 1.0

Chaque sommet représente un état et chaque ligne décrit une probabilité de passage d’un état vers un autre.

---

## Structure générale du code

Le projet est séparé en plusieurs fichiers :

- **main_markov.c**  
  Programme principal : lecture du graphe, affichage, Tarjan, diagramme de Hasse, matrice de transition, distribution stationnaire, période.

- **markov.c / markov.h**  
  Lecture du fichier, affichage de la liste d’adjacence, vérification Markov, export Mermaid, construction de la matrice de transition, opérations sur matrices (multiplication, puissances, convergence, sous-matrice, période).

- **hasse.c / hasse.h**  
  Récupération des classes via Tarjan, liens entre classes et export du diagramme de Hasse.

- **utils.c / utils.h**  
  Fonctions utilitaires (pile d’entiers, allocations, identifiants Mermaid, etc.).

---

## Compilation

### Avec CMake (recommandé)

Depuis la racine du projet :

```bash
mkdir build
cd build
cmake ..
make

Cela génère l’exécutable principal.

A### Avec gcc (alternative simple)

```bash
gcc -Wall -Wextra -Werror \
    main_markov.c markov.c hasse.c utils.c \
    -o markov


---

## Exécution du programme

Commande :

./markov mon_fichier.txt

Le programme affiche :
	•	la liste d’adjacence du graphe,
	•	la vérification de la somme des probabilités sortantes,
	•	les classes trouvées par Tarjan,
	•	des informations sur la matrice de transition (puissances, distribution stationnaire, période).

Il génère aussi deux fichiers :
	•	graph_mermaid.md : représentation du graphe,
	•	hasse_mermaid.md : diagramme de Hasse.

Les fichiers Mermaid générés peuvent être ouverts avec n’importe quel visualiseur compatible.
---

## Exécution du programme

Graphe
	•	Lecture du fichier et construction d’une liste d’adjacence.
	•	Vérification que chaque sommet a une somme de probabilités sortantes proche de 1 (tolérance 0.01).
	•	Export d’un fichier Mermaid pour visualiser le graphe.

Classes (Tarjan)
	•	Détection des composantes fortement connexes.
	•	Affichage des différentes classes.
	•	Déduction des liens entre classes.
	•	Suppression des liens redondants pour obtenir le diagramme de Hasse.
	•	Export en Mermaid du diagramme final.

Matrices
	•	Construction de la matrice de transition.
	•	Calcul de puissances (exemple : M³).
	•	Approximation d’une distribution stationnaire par itérations successives.
	•	Extraction d’une sous-matrice associée à une classe.
	•	Calcul de la période de cette classe.

---
Répartition du travail
