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

    if(!heap_validate_assignment(start) || !heap_validate_assignment(end)) {
        res = -EINVARG;
        goto out;
    }

    memset(heap, 0, sizeof(struct heap));
    heap->saddr = start;
    heap->table = table;

    res = heap_validate_table(start, end, table);
    if(res < 0) {
        goto out;
    }

    size_t table_size = sizeof(HEAP_BLOCK_ENTRY) * table->total_size;
    memset(table->entries, HEAP_BLOCK_FREE, table_size);

out:
    return res;
}

static uint32_t heap_align_value_to_upper(uint32_t val) {
    if((val % AETHER_HEAP_BLOCK_SIZE) == 0)
        return val;

    val = (val - (val % AETHER_HEAP_BLOCK_SIZE));
    val += AETHER_HEAP_BLOCK_SIZE;
    return val;
}

static int heap_get_entry_type(HEAP_BLOCK_ENTRY entry) {
    return entry & 0x0f;
}

int heap_get_start_block(struct heap *heap, uint32_t total_blocks) {
    struct heap_table *table = heap->table;
    int bc = 0; // Current block
    int bs = -1; // First block thats free

    for(size_t i = 0; i < table->total_size; i++) {
        if(heap_get_entry_type(table->entries[i]) != HEAP_BLOCK_FREE) {
            bc = 0;
            bs = -1;
            continue;
        }

        // If the current block is the first block
        if(bs == -1) {
            bs = i;
        }

        bc++;
        if(bc == total_blocks) {
            break;
        }
    }

    if(bs == -1) {
        return -ENOMEM;
    }

    return bs;
}

void *heap_block_to_addr(struct heap *heap, uint32_t block) {
    return heap->saddr + (block * AETHER_HEAP_BLOCK_SIZE);
}

void heap_mark_blocks_taken(struct heap *heap, uint32_t start_block, uint32_t total_blocks) {
    int end_block = (start_block + total_blocks) - 1;

    HEAP_BLOCK_ENTRY entry = HEAP_BLOCK_TAKEN | HEAP_BLOCK_IS_FIRST;
    if(total_blocks > 1) {
        entry |= HEAP_BLOCK_HAS_NEXT;
    }

    for(size_t i = start_block; i <= end_block; i++) {
        heap->table->entries[i] = entry;
        entry = HEAP_BLOCK_TAKEN;
        if(i != end_block - 1) {
            entry |= HEAP_BLOCK_HAS_NEXT;
        }
    }
}

void *heap_malloc_blocks(struct heap *heap, uint32_t blocks) {
    void *addr = NULL;
    int start_block = heap_get_start_block(heap, blocks);
    if(start_block < 0) {
        goto out;
    }
    addr = heap_block_to_addr(heap, start_block);
    heap_mark_blocks_taken(heap, start_block, blocks);

out:
    return addr;
}

int heap_addr_to_block(struct heap *heap, void *addr) {
    return ((int)(addr - heap->saddr) / AETHER_HEAP_BLOCK_SIZE);
}

void heap_mark_blocks_free(struct heap *heap, int start_block) {
    struct heap_table *table = heap->table;

    for(size_t i = start_block; i < (size_t) table->total_size; i++) {
        HEAP_BLOCK_ENTRY entry = table->entries[i];

        table->entries[i] = HEAP_BLOCK_FREE;
        if(!(entry & HEAP_BLOCK_HAS_NEXT)) {
            break;
        }
    }
}

void *heap_malloc(struct heap *heap, size_t size) {
    size_t aligned_size = heap_align_value_to_upper(size);
    uint32_t total_blocks = aligned_size / AETHER_HEAP_BLOCK_SIZE;

    return heap_malloc_blocks(heap, total_blocks);
}

void heap_free(struct heap *heap, void *ptr) {
    heap_mark_blocks_free(heap, heap_addr_to_block(heap, ptr));
}
