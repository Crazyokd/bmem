#include "bmem.h"

int main()
{
    bm_context_t *ctx = bm_context_register(8, 1024, 1024);
    if (ctx == NULL) return -1;
    int *a = bm_malloc(ctx, sizeof(int));
    bm_free_s(ctx, a, sizeof(int));
    bm_context_destroy(ctx);
    return 0;
}