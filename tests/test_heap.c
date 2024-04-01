#include "Unity/src/unity.h"
#include "../src/memory/heap/heap.h"

#define TEST_HEAP_SIZE 4096 * 100
#define TEST_HEAP_BLOCK_SIZE AETHER_HEAP_BLOCK_SIZE
#define TEST_HEAP_TOTAL_BLOCKS (TEST_HEAP_SIZE / TEST_HEAP_BLOCK_SIZE)

HEAP_BLOCK_ENTRY test_heap_table_entries[TEST_HEAP_TOTAL_BLOCKS];
struct heap_table test_heap_table = { test_heap_table_entries, TEST_HEAP_TOTAL_BLOCKS };
struct heap test_heap;
uint8_t test_heap_memory[TEST_HEAP_SIZE];

void tearDown(void) {}

void setUp(void) {
    heap_create(&test_heap, test_heap_memory, test_heap_memory + sizeof(test_heap_memory), &test_heap_table);
}

void test_heap_create(void) {
    TEST_ASSERT_NOT_NULL(test_heap.saddr);
    TEST_ASSERT_NOT_NULL(test_heap.table);
    TEST_ASSERT_EQUAL_PTR(test_heap_memory, test_heap.saddr);
    TEST_ASSERT_EQUAL_PTR(&test_heap_table, test_heap.table);
    TEST_ASSERT_EQUAL(TEST_HEAP_TOTAL_BLOCKS, test_heap.table->total_size);
    for(size_t i = 0; i < TEST_HEAP_TOTAL_BLOCKS; i++) {
        TEST_ASSERT_EQUAL_HEX8(HEAP_BLOCK_FREE, test_heap.table->entries[i]);
    }
}

void test_heap_malloc_and_free(void) {
    void* ptr1 = heap_malloc(&test_heap, 1); // Allocate 1 byte, but it should allocate at least one block
    TEST_ASSERT_NOT_NULL(ptr1);
    TEST_ASSERT_TRUE(((uintptr_t)ptr1 % TEST_HEAP_BLOCK_SIZE) == 0); // Check alignment

    // Free the block and check if it's marked free
    heap_free(&test_heap, ptr1);
    for(size_t i = 0; i < TEST_HEAP_TOTAL_BLOCKS; i++) {
        if (i == ((uintptr_t)ptr1 - (uintptr_t)test_heap_memory) / TEST_HEAP_BLOCK_SIZE) {
            TEST_ASSERT_EQUAL_HEX8(HEAP_BLOCK_FREE, test_heap.table->entries[i]);
        }
    }

    // Allocate memory that spans multiple blocks
    void* ptr2 = heap_malloc(&test_heap, TEST_HEAP_BLOCK_SIZE + 1); // This should span more than one block
    TEST_ASSERT_NOT_NULL(ptr2);
    heap_free(&test_heap, ptr2);
}

void test_heap_malloc_failure(void) {
    // Try to allocate more memory than available in the heap
    void* ptr = heap_malloc(&test_heap, TEST_HEAP_SIZE * 2); // Request more memory than what's available
    TEST_ASSERT_NULL(ptr);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_heap_create);
    RUN_TEST(test_heap_malloc_and_free);
    RUN_TEST(test_heap_malloc_failure);

    return UNITY_END();
}
