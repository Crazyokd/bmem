#include "bmem.h"

#include <stdlib.h>

#define UNIT_SIZE(s) (s + sizeof(bm_index_t))

static bm_table_t *bm_table_create(bm_context_t *ctx, size_t s)
{
    bm_table_t *table = ctx->tables[ctx->ctc] = malloc(sizeof(bm_table_t));
    if (!table) return NULL;

    /* initialize table */
    table->alloc = malloc(sizeof(alloc_queue_t));
    if (!table->alloc) {
        free(table);
        return NULL;
    }
    table->alloc->array = malloc(sizeof(bm_index_t) * ctx->iuc);
    if (!table->alloc->array) {
        free(table->alloc);
        free(table);
        return NULL;
    }
    void *pool = malloc(sizeof(bm_table_t *) + UNIT_SIZE(s) * ctx->iuc);
    if (!pool) {
        free(table->alloc->array);
        free(table->alloc);
        free(table);
        return NULL;
    }

    /* now we have allocated all needed space successfully */
    *(bm_table_t **)pool = table;
    /* initialize table pool */
    table->pool = pool + sizeof(bm_table_t *);
    for (int i = 0; i < ctx->iuc; i++) {
        *(bm_index_t *)table->pool = i;
        table->pool += UNIT_SIZE(s);
    }
    table->pool = pool + sizeof(bm_table_t *);
    /* initialize table alloc */
    for (int i = 1; i < ctx->iuc; i++) {
        table->alloc->array[i] = i;
    }
    table->alloc->head = 1;
    table->alloc->tail = ctx->iuc - 1;

    table->unit_cnt = ctx->iuc;
    table->unit_size = s;
    return table;
}

static void bm_table_destroy(bm_table_t *table)
{
    if (!table) return;
    /* we must offset pool to get real start point */
    free(table->pool - sizeof(bm_table_t *));
    free(table->alloc->array);
    free(table->alloc);
    free(table);
}

bm_context_t *bm_context_register(uint32_t max_table_cnt, size_t iuc,
                                  size_t max_unit_cnt)
{
    if (max_table_cnt == 0 || iuc == 0) return NULL;
    if (iuc > max_unit_cnt) return NULL;

    bm_context_t *ctx = malloc(sizeof(bm_context_t));
    if (!ctx) return NULL;
    ctx->tables = malloc(sizeof(bm_table_t *) * max_table_cnt);
    if (!ctx->tables) {
        free(ctx);
        return NULL;
    }
    ctx->mtc = max_table_cnt;
    ctx->iuc = iuc;
    ctx->muc = max_unit_cnt;
    ctx->ctc = 0;
    return ctx;
}

void bm_context_destroy(bm_context_t *ctx)
{
    if (!ctx) return;

    for (int i = 0; i < ctx->ctc; i++) {
        bm_table_destroy(ctx->tables[i]);
    }
    free(ctx->tables);
    free(ctx);
}

void *bm_malloc(bm_context_t *ctx, size_t s)
{
    if (!ctx || s == 0) return NULL;
    /* find table by t */
    for (int i = 0; i < ctx->ctc; i++) {
        if (ctx->tables[i]->unit_size == s) {
            if (ctx->tables[i]->alloc->head == ctx->tables[i]->alloc->tail) {
                /* The memory pool has been exhausted */
                /* todo: consider expansion */
                return NULL; // now just do nothing
            }
            /* alloc memory */
            size_t offset =
                ctx->tables[i]->alloc->array[ctx->tables[i]->alloc->head]
                    * UNIT_SIZE(s)
                + sizeof(bm_index_t);
            ctx->tables[i]->alloc->head++;
            if (ctx->tables[i]->alloc->head >= ctx->tables[i]->unit_cnt) {
                /* unlikely */
                ctx->tables[i]->alloc->head %= ctx->tables[i]->unit_cnt;
            }
            return ctx->tables[i]->pool + offset;
        }
    }
    /* here we will create table */
    if (ctx->ctc >= ctx->mtc) return NULL;
    bm_table_t *table = bm_table_create(ctx, s);
    if (table) {
        ctx->tables[ctx->ctc++] = table;
        return table->pool + sizeof(bm_index_t);
    }
    return NULL;
}

static void bm_free_t(bm_table_t *table, size_t idx)
{
    if (++table->alloc->tail >= table->unit_cnt) {
        table->alloc->tail %= table->unit_cnt;
    }
    table->alloc->array[table->alloc->tail] = idx;
}

void bm_free_s(bm_context_t *ctx, void *ptr, size_t s)
{
    if (ctx == NULL || ptr == NULL || s == 0) return;
    /* point to real start point */
    bm_index_t *idx_ptr = (bm_index_t *)(ptr - sizeof(bm_index_t));
    /* point to the address of table */
    bm_table_t *table =
        *(bm_table_t **)((void *)idx_ptr - UNIT_SIZE(s) * (*idx_ptr)
                         - sizeof(bm_table_t *));
    /* free ptr */
    bm_free_t(table, *idx_ptr);
}

void bm_free(bm_context_t *ctx, void *ptr)
{
    if (ctx == NULL || ptr == NULL) return;

    bm_index_t *idx_ptr = (bm_index_t *)(ptr - sizeof(bm_index_t));
    for (int i = 0; i < ctx->ctc; i++) {
        size_t offset = *idx_ptr * UNIT_SIZE(ctx->tables[i]->unit_size)
                      + sizeof(bm_index_t);
        if (ctx->tables[i]->pool + offset == ptr) {
            /* now we find the real table */
            bm_free_t(ctx->tables[i], *idx_ptr);
            break;
        }
    }
}
