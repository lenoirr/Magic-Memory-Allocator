#ifndef MAIN_H
#define MAIN_H

#include <stddef.h>

// Structure Definitions
typedef struct Block {
    size_t size;
    int free;
    struct Block* next;
} Block;

// Block size constant
#define MEMORY_POOL_SIZE 1024
#define BLOCK_SIZE sizeof(Block)

extern char memory_pool[MEMORY_POOL_SIZE];          // Simulated Heap
extern Block* free_list;                            // Head of the free list 

// Exposed Function Declarations
void initialize_memory_pool();
void* magic_malloc(size_t size);
void magic_free(void* ptr);
void visualize_memory_pool();

#endif // MAIN_H
