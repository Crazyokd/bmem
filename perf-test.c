#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "bmem.h"

#define ALLOC_CNT 102400000

static void test_interleaved_fashion()
{
    clock_t begin, end;
    double time_spent;
    int **a = malloc(sizeof(int *) * ALLOC_CNT);

    /* normal malloc and free */
    begin = clock();

    for (uint32_t i = 0; i < ALLOC_CNT; i++) {
        a[i] = malloc(sizeof(int));
        free(a[i]);
    }

    end = clock();

    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Normal interleaved fashion: Time spent: %f seconds\n", time_spent);

    /* Bmem */
    begin = clock();

    bm_context_t *ctx = bm_context_register(8, 1024, 1024);
    for (uint32_t i = 0; i < ALLOC_CNT; i++) {
        a[i] = bm_malloc(ctx, sizeof(int));
        bm_free_s(a[i], sizeof(int));
    }
    bm_context_destroy(ctx);

    end = clock();

    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Bmem interleaved fashion: Time spent: %f seconds\n", time_spent);

    free(a);
}

static void test_bulk_fashion()
{
    clock_t begin, end;
    double time_spent;
    int **a = malloc(sizeof(int *) * ALLOC_CNT);

    /* normal malloc and free */
    begin = clock();

    for (uint32_t i = 0; i < ALLOC_CNT; i++) {
        a[i] = malloc(sizeof(int));
    }
    for (uint32_t i = 0; i < ALLOC_CNT; i++) {
        free(a[i]);
    }

    end = clock();

    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Normal bulk fashion: Time spent: %f seconds\n", time_spent);

    /* Bmem */
    begin = clock();

    bm_context_t *ctx = bm_context_register(8, 1024, 1024);
    for (uint32_t i = 0; i < ALLOC_CNT; i++) {
        a[i] = bm_malloc(ctx, sizeof(int));
    }
    for (uint32_t i = 0; i < ALLOC_CNT; i++) {
        bm_free_s(a[i], sizeof(int));
    }
    bm_context_destroy(ctx);

    end = clock();

    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Bmem bulk fashion: Time spent: %f seconds\n", time_spent);

    free(a);
}

int main()
{
    test_interleaved_fashion();
    test_bulk_fashion();
    return 0;
}
