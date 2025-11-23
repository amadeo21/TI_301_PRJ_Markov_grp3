PARTIE 3 - Instructions rapides

1) Compilation (dans le dossier contenant tous les .c/.h et le Makefile):
   make

   Cela produit 3 exécutables : part1, part2, part3

2) Exécution exemple pour PARTIE 3 :
   ./part3 exemple1.txt 3 0.001:1000

   - "3" : calcule M^3 (affiche M^3)
   - "0.001:1000" : eps = 0.001, max_iter = 1000 pour les itérations de convergence

3) Ce que fait le programme :
   - construit la matrice M depuis le fichier d'adjacence (format du sujet)
   - affiche M
   - calcule M^k et l'affiche
   - évolue une distribution initiale Pi0 = (1,0,0,...) jusqu'à convergence (ou max_iter)
   - calcule la partition (Tarjan) et les classes
   - extrait les sous-matrices des classes persistantes et calcule la distribution stationnaire par classe (itération)
   - calcule la période (fonction getPeriod) pour chaque classe (défi)
   - affiche les résultats et la stationnarité étendue à tout le graphe

4) Notes d'implémentation :
   - Les matrices sont en float, stockage contigu.
   - Les tolérances et max_iter contrôlent la détection de la stationnarité.
   - Pour des graphes plus grands, tu peux augmenter max_iter ou diminuer eps.
