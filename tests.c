#include "test.h"
#include "main.h"
#include <time.h>
#include <windows.h>
#include <string.h>
// Performance Testings

static void log_performance(const char* operation, size_t size, LARGE_INTEGER start, LARGE_INTEGER end, LARGE_INTEGER frequency) {
    double time_taken = (double)(end.QuadPart - start.QuadPart) * 1000.0 / frequency.QuadPart;
    printf("%s of %zu bytes took %.5f ms\n", operation, size, time_taken);
}

// Test Implementations

void test_allocate_full_pool() {
    TEST_START("Allocate Full Pool");

    initialize_memory_pool();
    
    LARGE_INTEGER frequency, start_time, end_time;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start_time);

    void* ptr = magic_malloc(MEMORY_POOL_SIZE - BLOCK_SIZE);

    QueryPerformanceCounter(&end_time);
    log_performance("Allocate Full Pool", 0, start_time, end_time, frequency);

    assert(ptr != NULL && "Failed to allocate full pool.");

    Block* block = (Block*)((char*)ptr - BLOCK_SIZE);
    assert(block->size == MEMORY_POOL_SIZE - BLOCK_SIZE && block->free == 0);

    TEST_SUCCESS("Allocate Full Pool");
    
    visualize_memory_pool();
    clear_memory_pool();
}

void test_free_middle_block() {
    TEST_START("Free Middle Block");

    initialize_memory_pool();
    void* ptr1 = magic_malloc(128);
    void* ptr2 = magic_malloc(256);
    void* ptr3 = magic_malloc(512);

    assert(ptr1 && ptr2 && ptr3 && "Failed to allocate multiple blocks.");

    magic_free(ptr2);

    Block* block = (Block*)((char*)ptr2 - BLOCK_SIZE);
    assert(block->free == 1 && "Middle block was not freed.");
    assert(block->size == 256);
    assert(block->next->free == 1 && "Did not point freed block node to next free block");

    TEST_SUCCESS("Free Middle Block");
    visualize_memory_pool();
    clear_memory_pool();
}

void test_allocate_zero_bytes() {
    TEST_START("Allocate Zero Bytes");

    initialize_memory_pool();
    void* ptr = magic_malloc(0);
    assert(ptr == NULL && "Allocating zero bytes should fail.");

    TEST_SUCCESS("Allocate Zero Bytes");
    visualize_memory_pool();
    clear_memory_pool();
}

void test_allocate_larger_than_pool() {
    TEST_START("Allocate Larger Than Pool");

    initialize_memory_pool();
    void* ptr = magic_malloc(MEMORY_POOL_SIZE);
    assert(ptr == NULL && "Allocating more memory than pool size should fail.");

    TEST_SUCCESS("Allocate Larger Than Pool");
    visualize_memory_pool();
    clear_memory_pool();
}

void test_allocate_and_free_all() {
    TEST_START("Allocate and Free All");

    initialize_memory_pool();
    void* ptr1 = magic_malloc(MEMORY_POOL_SIZE - BLOCK_SIZE);

    assert(ptr1 && "Failed to allocate memory blocks.");

    magic_free(ptr1);

    assert(free_list->size == MEMORY_POOL_SIZE - BLOCK_SIZE && "Incorrect free block size after freeing all blocks.");
    assert(free_list->next == NULL && "Free list should only contain one block.");
    assert(free_list->free == 1 && "Free block should be marked as free.");

    TEST_SUCCESS("Allocate and Free All");
    visualize_memory_pool();
    clear_memory_pool();
}

void test_left_coalescing() {
    TEST_START("Left Coalescing");

    initialize_memory_pool();

    void* ptr1 = magic_malloc(128);
    void* ptr2 = magic_malloc(256);
    void* ptr3 = magic_malloc(128);
    void* ptr4 = magic_malloc(64);

    magic_free(ptr2);

    assert(free_list == (ptr2 - BLOCK_SIZE) && "Free list not updated to start at pt2");
    magic_free(ptr3);

    Block* block = (Block*)((char*)ptr2 - BLOCK_SIZE);
    assert(block->size == 256 + BLOCK_SIZE + 128 && "Left coalescing failed.");
    assert(block->free == 1 && "Block should be marked as free.");

    Block* block4 = (Block*)((char*)ptr4 - BLOCK_SIZE);
    assert(block->next == block4->next && "Failed to update next in new free block.");

    TEST_SUCCESS("Left Coalescing");
    visualize_memory_pool();
    clear_memory_pool();
}

void test_right_coalescing() {
    TEST_START("Right Coalescing");

    initialize_memory_pool();

    void* ptr1 = magic_malloc(128);
    void* ptr2 = magic_malloc(256);
    void* ptr3 = magic_malloc(128);
    void* ptr4 = magic_malloc(64);

    magic_free(ptr3);
    assert(free_list == (ptr3 - BLOCK_SIZE) && "Free list not updated to start at pt3");

    magic_free(ptr2);

    Block* block = (Block*)((char*)ptr2 - BLOCK_SIZE);
    assert(block->size == 256 + BLOCK_SIZE + 128 && "Right coalescing failed.");
    assert(block->free == 1 && "Block should be marked as free.");
    assert(free_list == (ptr2 - BLOCK_SIZE) && "Free list not updated to start at pt2");

    Block* block4 = (Block*)((char*)ptr4 - BLOCK_SIZE);
    assert(block->next == block4->next && "ptr2 next not updated to next free block");

    TEST_SUCCESS("Right Coalescing");
    visualize_memory_pool();
    clear_memory_pool();
}

void test_full_coalescing() {
    TEST_START("Full Coalescing");

    initialize_memory_pool();

    void* ptr1 = magic_malloc(128);
    void* ptr2 = magic_malloc(256);
    void* ptr3 = magic_malloc(128);
    void* ptr4 = magic_malloc(64);

    magic_free(ptr2);
    magic_free(ptr4);

    Block* block4 = (Block*)((char*)ptr4 - BLOCK_SIZE);
    assert(block4->size == 64 + BLOCK_SIZE + 328 && "right coalescing failed");
    magic_free(ptr3);


    Block* block = (Block*)((char*)ptr2 - BLOCK_SIZE);
    assert(free_list == block && "Free list not updated to start at block2");
    assert(block->size == 256 + BLOCK_SIZE + 128 + BLOCK_SIZE + 64 + BLOCK_SIZE + 328 && "Full coalescing failed.");
    assert(block->free == 1 && "Block should be marked as free.");
    assert(block->next == NULL && "Block not updated to point to null");

    TEST_SUCCESS("Full Coalescing");
    visualize_memory_pool();
    clear_memory_pool();
}

void test_double_free() {
    TEST_START("Double Free");

    initialize_memory_pool();
    void* ptr = magic_malloc(128);
    magic_free(ptr);

    Block* block = (Block*)((char*)ptr - BLOCK_SIZE);
    assert(block->free == 1 && "Block should be marked as free.");

    printf("Attempting double free...\n");
    magic_free(ptr); // Should not crash or corrupt memory

    assert(block->free == 1 && "Double free should not corrupt block state.");

    TEST_SUCCESS("Double Free");

    visualize_memory_pool();
    clear_memory_pool();
}

void clear_memory_pool() {
    for (int i = 0; i < MEMORY_POOL_SIZE; i++) {
        memory_pool[i] = 0;
    }
    initialize_memory_pool();
}

void test_calloc() {
    TEST_START("Calloc");

    initialize_memory_pool();
    void* ptr = magic_calloc(1, 128);
    assert(ptr != NULL);

    Block* block = (Block*)((char*)ptr - BLOCK_SIZE);
    assert(block->size == 128 && block->free == 0);

    TEST_SUCCESS("Calloc");

    visualize_memory_pool();
    clear_memory_pool();
}

void test_realloc_null_pointer() {
    TEST_START("Realloc Null Pointer");

    initialize_memory_pool();
    void* ptr = magic_realloc(NULL, 128);
    assert(ptr != NULL && "Failed to allocate memory with realloc for NULL pointer.");
    
    Block* block = (Block*)((char*)ptr - BLOCK_SIZE);
    assert(block->size == 128 && block->free == 0);

    TEST_SUCCESS("Realloc Null Pointer");
    visualize_memory_pool();
    clear_memory_pool();
}

void test_realloc_smaller_size() {
    TEST_START("Realloc Smaller Size");

    initialize_memory_pool();
    void* ptr = magic_malloc(256);
    assert(ptr != NULL && "Failed to allocate initial memory.");

    *(int*)ptr = 3;

    void* new_ptr = magic_realloc(ptr, 128);
    assert(new_ptr == ptr && "Realloc to smaller size should return the same pointer.");
    assert(*(int*)new_ptr == 3 && "Realloc did not properly copy data.");

    Block* block = (Block*)((char*)new_ptr - BLOCK_SIZE);
    assert(block->size == 256 && block->free == 0);

    TEST_SUCCESS("Realloc Smaller Size");
    visualize_memory_pool();
    clear_memory_pool();
}

void test_realloc_larger_size() {
    TEST_START("Realloc Larger Size");

    initialize_memory_pool();
    void* ptr = magic_malloc(128);
    assert(ptr != NULL && "Failed to allocate initial memory.");

    void* new_ptr = magic_realloc(ptr, 256);
    assert(new_ptr != NULL && "Failed to realloc to larger size.");
    
    Block* block = (Block*)((char*)new_ptr - BLOCK_SIZE);
    assert(block->size == 256 && block->free == 0);

    TEST_SUCCESS("Realloc Larger Size");
    visualize_memory_pool();
    clear_memory_pool();
}

void test_realloc_free_block() {
    TEST_START("Realloc Free Block");

    initialize_memory_pool();
    void* ptr = magic_malloc(128);
    assert(ptr != NULL && "Failed to allocate initial memory.");

    magic_free(ptr);
    void* new_ptr = magic_realloc(ptr, 256);
    assert(new_ptr != NULL && "Failed to realloc a free block.");

    Block* block = (Block*)((char*)new_ptr - BLOCK_SIZE);
    assert(block->size == 256 && block->free == 0);

    TEST_SUCCESS("Realloc Free Block");
    visualize_memory_pool();
    clear_memory_pool();
}

void test_realloc_zero_size() {
    TEST_START("Realloc Zero Size");

    initialize_memory_pool();
    void* ptr = magic_malloc(128);
    assert(ptr != NULL && "Failed to allocate initial memory.");

    void* new_ptr = magic_realloc(ptr, 0);
    assert(new_ptr == NULL && "Realloc to zero size should return NULL.");

    TEST_SUCCESS("Realloc Zero Size");
    visualize_memory_pool();
    clear_memory_pool();
}
/// ------------------------------- PERFORMANCE TESTS ------------------------------- //

void test_worst_case_malloc() {
    TEST_START("Worst Case Malloc");

    initialize_memory_pool();
    
    LARGE_INTEGER frequency, start_time, end_time;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start_time);

    // Allocate the entire memory pool in 1 Byte increments
    size_t alloc_size = 1;
    void* ptr = NULL;
    size_t total_allocated = (MEMORY_POOL_SIZE / BLOCK_SIZE) - 1;
    while (1) {
        ptr = magic_malloc(1);
        if (!ptr) break;
    }

    QueryPerformanceCounter(&end_time);
    log_performance("Worst Case Malloc", total_allocated, start_time, end_time, frequency);

    TEST_SUCCESS("Worst Case Malloc");
    //visualize_memory_pool();
    clear_memory_pool();
}

void test_worst_case_free() {
    TEST_START("Worst Case Free");

    initialize_memory_pool();

    // Allocate the entire memory pool in 1 Byte increments
    size_t alloc_size = 1;
    void* ptr = NULL;
    size_t total_allocated = 0;
    while (1) {
        ptr = magic_malloc(1);
        total_allocated++;
        if (!ptr) break;
    }

    LARGE_INTEGER frequency, start_time, end_time;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start_time);

    // Free the entire memory pool in 1 Byte increments
    Block* block_to_free = (Block*)memory_pool;
    while ((char*)block_to_free < memory_pool + MEMORY_POOL_SIZE) {
        Block* next_block = (Block*)((char*)block_to_free + BLOCK_SIZE + block_to_free->size);
        magic_free((void*)(block_to_free + 1));
        block_to_free = next_block;
    }

    QueryPerformanceCounter(&end_time);
    log_performance("Worst Case Free", 0, start_time, end_time, frequency);

    TEST_SUCCESS("Worst Case Free");
    visualize_memory_pool();
    clear_memory_pool();
}

void test_realloc(){
    TEST_START("Realloc");

    initialize_memory_pool();
    void* ptr = magic_malloc(128);
    assert(ptr != NULL && "Failed to allocate initial memory.");

    void* new_ptr = magic_realloc(ptr, 256);
    assert(new_ptr != NULL && "Failed to realloc to larger size.");
    
    Block* block = (Block*)((char*)new_ptr - BLOCK_SIZE);
    assert(block->size == 256 && block->free == 0);

    TEST_SUCCESS("Realloc");
    visualize_memory_pool();
    clear_memory_pool();
}

// Run all tests
void run_all_tests() {
    test_allocate_full_pool();
    test_free_middle_block();
    test_allocate_zero_bytes();
    test_allocate_larger_than_pool();
    test_allocate_and_free_all();
    //test_fragmentation_and_coalescing();
    test_double_free();
    test_calloc();
    test_realloc_null_pointer();
    test_realloc_smaller_size();
    test_realloc_larger_size();
    test_realloc_free_block();
    test_realloc_zero_size();
}

void run_performace_tests(){
    test_worst_case_malloc();
    test_worst_case_free();
}

void run_coalesing_tests(){
    test_right_coalescing();
    test_left_coalescing();
    test_full_coalescing();
}
