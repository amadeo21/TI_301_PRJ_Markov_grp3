#include <stdlib.h>
#include <string.h>
#include "utils.h"

t_int_stack *stack_create(int capacity)
{
    t_int_stack *s = malloc(sizeof(t_int_stack));
    if (!s) {
        perror("malloc stack");
        exit(EXIT_FAILURE);
    }
    s->data = malloc(sizeof(int) * capacity);
    if (!s->data) {
        perror("malloc stack data");
        exit(EXIT_FAILURE);
    }
    s->top = -1;
    s->capacity = capacity;
    return s;
}

void stack_free(t_int_stack *stack)
{
    if (!stack) return;
    free(stack->data);
    free(stack);
}

void stack_push(t_int_stack *stack, int value)
{
    if (stack->top + 1 >= stack->capacity) {
        stack->capacity *= 2;
        int *new_data = realloc(stack->data, sizeof(int) * stack->capacity);
        if (!new_data) {
            perror("realloc stack");
            exit(EXIT_FAILURE);
        }
        stack->data = new_data;
    }
    stack->data[++stack->top] = value;
}

int stack_pop(t_int_stack *stack)
{
    if (stack->top < 0)
        return -1;
    return stack->data[stack->top--];
}

int stack_is_empty(const t_int_stack *stack)
{
    return (stack->top < 0);
}

char *getId(int num)
{
    if (num <= 0) num = 1;
    char buff[32];
    int i = 0;
    int n = num;
    while (n > 0 && i < (int)sizeof(buff) - 1) {
        n--;
        int r = n % 26;
        buff[i++] = (char)('A' + r);
        n /= 26;
    }
    buff[i] = '\0';
    for (int j = 0; j < i / 2; ++j) {
        char tmp = buff[j];
        buff[j] = buff[i - 1 - j];
        buff[i - 1 - j] = tmp;
    }
    char *res = malloc((size_t)i + 1);
    if (!res) {
        perror("malloc getId");
        exit(EXIT_FAILURE);
    }
    strcpy(res, buff);
    return res;
}

float *calloc_float_array(int n)
{
    float *arr = calloc((size_t)n, sizeof(float));
    if (!arr) {
        perror("calloc float array");
        exit(EXIT_FAILURE);
    }
    return arr;
}

int *calloc_int_array(int n)
{
    int *arr = calloc((size_t)n, sizeof(int));
    if (!arr) {
        perror("calloc int array");
        exit(EXIT_FAILURE);
    }
    return arr;
}
