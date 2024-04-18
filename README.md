# Bmem
batch memory - 批量内存

# Usage
```c
bm_context_t *ctx = bm_context_register(8, 1024, 1024);
if (ctx == NULL) return -1;
int *a = bm_malloc(ctx, sizeof(int));
// now you can use a as a normal pointer
bm_free_s(ctx, a, sizeof(int));
bm_context_destroy(ctx);
```

# Target
- allocate and release memory with O(1) complexity

# ROADMAP
- add performance test
- support expansion automatically
- using hash for boosting table query speed
- support multithread
