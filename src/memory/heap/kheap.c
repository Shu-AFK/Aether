#include "kheap.h"
#include "heap.h"
#include "config.h"
#include "kernel.h"
#include "status.h"

struct heap kernel_heap;
struct heap_table kernel_heap_table;

int kheap_init() {
    int total_table_entries = AETHER_HEAP_SIZE_BYTES / AETHER_HEAP_BLOCK_SIZE;
    kernel_heap_table.entries = (HEAP_BLOCK_ENTRY *) AETHER_HEAP_TABLE_ADDRESS;
    kernel_heap_table.total_size = total_table_entries;

    void *end = (void *) (AETHER_HEAP_ADDRESS + AETHER_HEAP_SIZE_BYTES);
    int res = heap_create(&kernel_heap, (void *) AETHER_HEAP_ADDRESS, end, &kernel_heap_table);
    if(res < 0) {
        print("Failed to create heap!\n");
        return -EHEAPCREATE;
    }

    return AETHER_OK;
}

void *kmalloc(size_t size) {
    return heap_malloc(&kernel_heap, size);
}

void kfree(void *addr) {
    heap_free(&kernel_heap, addr);
}
