#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <assert.h>
#include "main.h" 

// Test Macros
#define TEST_START(name) printf("Running Test: %s\n", name)
#define TEST_SUCCESS(name) printf("✔️  Test Passed: %s\n\n", name)


// Test Prototypes
void test_allocate_full_pool();
void test_free_middle_block();
void test_allocate_zero_bytes();
void test_allocate_larger_than_pool();
void test_allocate_and_free_all();
void test_fragmentation_and_coalescing();
void test_double_free();
void test_calloc();
void test_realloc_null_pointer();
void test_realloc_smaller_size();
void test_realloc_larger_size();
void test_realloc_free_block();
void test_realloc_zero_size();

// Helper Function Prototypes
void clear_memory_pool();

// Function to run all tests
void run_all_tests();

#endif // TEST_H
