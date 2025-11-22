#include "tarjan.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ---------- utils pour t_class / t_partition ---------- */

void init_t_class(t_class *c) {
    c->vertices = NULL;
    c->size = 0;
    c->capacity = 0;
    c->name[0] = '\0';
}

void free_t_class(t_class *c) {
    if (c->vertices) free(c->vertices);
    c->vertices = NULL;
    c->size = 0;
    c->capacity = 0;
}

void init_t_partition(t_partition *p) {
    p->classes = NULL;
    p->count = 0;
    p->capacity = 0;
}

void free_t_partition(t_partition *p) {
    if (!p) return;
    for (int i = 0; i < p->count; ++i) free_t_class(&p->classes[i]);
    if (p->classes) free(p->classes);
    p->classes = NULL;
    p->count = 0;
    p->capacity = 0;
}

void add_vertex_to_class(t_class *c, int v) {
    if (c->size + 1 > c->capacity) {
        int newcap = (c->capacity == 0) ? 4 : c->capacity * 2;
        c->vertices = (int*)realloc(c->vertices, newcap * sizeof(int));
        c->capacity = newcap;
    }
    c->vertices[c->size++] = v;
}

void add_class_to_partition(t_partition *p, t_class *c) {
    if (p->count + 1 > p->capacity) {
        int newcap = (p->capacity == 0) ? 4 : p->capacity * 2;
        p->classes = (t_class*)realloc(p->classes, newcap * sizeof(t_class));
        /* initialize new slots */
        for (int i = p->capacity; i < newcap; ++i) init_t_class(&p->classes[i]);
        p->capacity = newcap;
    }
    /* move c into partition (copy struct) */
    p->classes[p->count] = *c;
    /* ensure the moved-from c won't free the array when freed by caller */
    c->vertices = NULL;
    c->size = 0;
    c->capacity = 0;
    p->count++;
}

/* ---------- Implémentation de Tarjan (récursive) ---------- */

typedef struct {
    int *index;    /* index[v] = numéro DFS, -1 si non visité */
    int *low;
    int *onstack;  /* 0/1 */
    int *stack;    /* stack of vertices */
    int stack_top;
    int current_index;
    t_partition *part; /* where to push new classes */
    AdjList *g;
} tarjan_ctx;

/* push/pop stack */
static void push_stack(tarjan_ctx *ctx, int v) {
    ctx->stack[ctx->stack_top++] = v;
    ctx->onstack[v] = 1;
}
static int pop_stack(tarjan_ctx *ctx) {
    if (ctx->stack_top == 0) return -1;
    int v = ctx->stack[--ctx->stack_top];
    ctx->onstack[v] = 0;
    return v;
}

static void strongconnect(tarjan_ctx *ctx, int v) {
    ctx->index[v] = ctx->current_index;
    ctx->low[v] = ctx->current_index;
    ctx->current_index++;
    push_stack(ctx, v);

    /* for each neighbour w of v */
    Cell *cur = ctx->g->lists[v].head;
    while (cur) {
        int w = cur->arrivee;
        if (ctx->index[w] == -1) {
            strongconnect(ctx, w);
            if (ctx->low[w] < ctx->low[v]) ctx->low[v] = ctx->low[w];
        } else if (ctx->onstack[w]) {
            if (ctx->index[w] < ctx->low[v]) ctx->low[v] = ctx->index[w];
        }
        cur = cur->next;
    }

    /* If v is a root node, pop the stack and generate an SCC */
    if (ctx->low[v] == ctx->index[v]) {
        t_class comp;
        init_t_class(&comp);
        /* pop until v */
        while (1) {
            int w = pop_stack(ctx);
            if (w == -1) break; /* shouldn't happen */
            add_vertex_to_class(&comp, w);
            if (w == v) break;
        }
        /* sort vertices within class? not necessary */
        add_class_to_partition(ctx->part, &comp);
    }
}

t_partition_result tarjan_partition(const AdjList *g_in) {
    /* Prepare context */
    AdjList g_copy = *(AdjList*)g_in; /* we only read, safe to cast away const for pointer usage */
    int n = g_copy.n;
    tarjan_ctx ctx;
    ctx.index = (int*)malloc((n+1) * sizeof(int));
    ctx.low = (int*)malloc((n+1) * sizeof(int));
    ctx.onstack = (int*)malloc((n+1) * sizeof(int));
    ctx.stack = (int*)malloc((n+1) * sizeof(int));
    ctx.stack_top = 0;
    ctx.current_index = 0;
    ctx.g = &g_copy;

    for (int i = 1; i <= n; ++i) { ctx.index[i] = -1; ctx.low[i] = -1; ctx.onstack[i] = 0; }

    t_partition part;
    init_t_partition(&part);
    ctx.part = &part;

    for (int v = 1; v <= n; ++v) {
        if (ctx.index[v] == -1) strongconnect(&ctx, v);
    }

    /* construct vertex -> class table */
    int *v2c = (int*)malloc((n+1) * sizeof(int));
    for (int i = 0; i <= n; ++i) v2c[i] = 0;
    for (int ci = 0; ci < part.count; ++ci) {
        t_class *c = &part.classes[ci];
        /* name */
        snprintf(c->name, sizeof(c->name), "C%d", ci+1);
        for (int k = 0; k < c->size; ++k) {
            int v = c->vertices[k];
            if (v >=1 && v <= n) v2c[v] = ci+1; /* classes numbered 1..count */
        }
    }

    /* prepare result */
    t_partition_result res;
    res.partition = part;
    res.vertex_to_class = v2c;

    free(ctx.index); free(ctx.low); free(ctx.onstack); free(ctx.stack);

    return res;
}

void free_partition_result(t_partition_result *res) {
    if (!res) return;
    free_t_partition(&res->partition);
    if (res->vertex_to_class) free(res->vertex_to_class);
    res->vertex_to_class = NULL;
}
