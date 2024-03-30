#include "heap.h"
#include "kernel.h"
#include "status.h"
#include "memory/memory.h"
#include <stdbool.h>

static int heap_validate_table(void *start, void *end, struct heap_table *table) {
    int res = 0;

    size_t table_size = (size_t) (end - start);
    size_t total_blocks = table_size / AETHER_HEAP_BLOCK_SIZE;
    if(table->total_size != total_blocks) {
        res -= EINVARG;
        goto out;
    }

out:
    return res;
}

static bool heap_validate_assignment(void *ptr) {
    return ((unsigned int) ptr % AETHER_HEAP_BLOCK_SIZE) == 0;
}

int heap_create(struct heap *heap, void *start, void *end, struct heap_table *table) {
    int res = 0;

    if(heap_validate_assignment(start) || heap_validate_assignment(end)) {
        res = - EINVARG;
        goto out;
    }
    memset(heap, 0, sizeof(struct heap));
    heap->saddr = start;
    heap->table = table;

    res = heap_validate_table(start, end, table);
    if(res < 0)
        goto out;

    size_t table_size = sizeof(HEAP_BLOCK_ENTRY) * table->total_size;
    memset(table->entries, HEAP_BLOCK_FREE, table_size);

out:
    return res;
}

static uint32_t heap_align_value_to_upper(uint32_t val) {
    if(val % AETHER_HEAP_BLOCK_SIZE == 0)
        return val;

    val -= (val % AETHER_HEAP_BLOCK_SIZE);
    val += AETHER_HEAP_BLOCK_SIZE;
    return val;
}

void *heap_malloc(size_t size) {
    size_t aligned_size = heap_align_value_to_upper(size);
    uint32_t total_blocks = aligned_size / AETHER_HEAP_BLOCK_SIZE;

    return NULL;
}

void heap_free(void *ptr) {

}
