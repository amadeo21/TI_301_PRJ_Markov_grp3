#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

/**
 * @brief Structure de pile d'entiers (utilisée par Tarjan).
 */
typedef struct s_int_stack {
    int *data;
    int top;
    int capacity;
} t_int_stack;

/**
 * @brief Initialise une pile vide.
 */
t_int_stack *stack_create(int capacity);

/**
 * @brief Libère la mémoire de la pile.
 */
void stack_free(t_int_stack *stack);

/**
 * @brief Empile un entier sur la pile.
 */
void stack_push(t_int_stack *stack, int value);

/**
 * @brief Dépile un entier. Si la pile est vide, retourne -1.
 */
int stack_pop(t_int_stack *stack);

/**
 * @brief Renvoie 1 si la pile est vide, 0 sinon.
 */
int stack_is_empty(const t_int_stack *stack);

/**
 * @brief Renvoie une chaîne d'identifiant pour Mermaid à partir d'un entier :
 * 1 -> "A", 2 -> "B", ..., 26 -> "Z", 27 -> "AA", etc.
 * La chaîne est allouée dynamiquement, à libérer par l'appelant.
 */
char *getId(int num);

/**
 * @brief Alloue un tableau de float initialisé à 0.0.
 */
float *calloc_float_array(int n);

/**
 * @brief Alloue un tableau d'int initialisé à 0.
 */
int *calloc_int_array(int n);

#endif // UTILS_H