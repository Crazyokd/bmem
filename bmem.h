#ifndef _B_MEM_
#define _B_MEM_

#include <stddef.h>
#include <stdint.h>

typedef uint32_t bm_index_t;

typedef struct alloc_queue_s {
    uint32_t head; /* the head index of array */
    uint32_t tail; /* the tail index of array */
    bm_index_t *array; /* point to alloc array */
} alloc_queue_t;

typedef struct bm_table_s {
    /**
     * point to real memory pool
     * !!! offset one pointer size to the left to point to the current table
     */
    void *pool;
    size_t unit_cnt; /* current unit cnt of array */
    size_t unit_size; /* size of unit */
    alloc_queue_t *alloc; /* point to alloc_queue */
    struct bm_table_s *next;
} bm_table_t;

typedef struct bm_context_s {
    size_t iuc; /* init unit cnt */
    size_t muc; /* max unit cnt */
    uint32_t mtc; /* max table cnt */
    uint32_t ctc; /* current table cnt */
    bm_table_t **tables;
} bm_context_t;

#define GET_SIZE_BY_TYPE(t_ptr) sizeof(*t_ptr)
/* API */
bm_context_t *bm_context_register(uint32_t max_table_cnt, size_t iuc,
                                  size_t max_unit_cnt);
void bm_context_destroy(bm_context_t *ctx);
void *bm_malloc(bm_context_t *ctx, size_t t);
void bm_free(bm_context_t *ctx, void *ptr);
void bm_free_s(void *ptr, size_t s);

#endif
