#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include <string.h>
#include "test.h"
#include <time.h>

#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))  // Ensures size is a multiple of the alignment

char memory_pool[MEMORY_POOL_SIZE];          // Simulated Heap
Block* free_list = (Block*)memory_pool;      // linked list to track free memory blocks

// Initializing the first block of metaData. 
void initialize_memory_pool() {
    free_list = (Block*)memory_pool; // free_list points to the start of the memory pool
    free_list->size = MEMORY_POOL_SIZE - BLOCK_SIZE;    
    free_list->next = NULL;                              
    free_list->free = 1;                                
}
int coalesce_right(Block* next_free, Block* block) {
    if ((Block*)((char*)block + BLOCK_SIZE + block->size) == next_free) {
        block->size += BLOCK_SIZE + next_free->size;
        block->next = next_free->next;
        return 1;
    }
    return 0;
}

int coalesce_left(Block* prev, Block* block) {
    if ((Block*)((char*)prev + BLOCK_SIZE + prev->size) == block) { // if previous free block is adjacent to the current block
        prev->size += BLOCK_SIZE + block->size;
        //prev->next = block->next;
        return 1;
    }
    return 0;
}


void insert_block(Block *block_to_free) {
    Block *prev = NULL;
    Block *current = free_list;

    // Traverse the free list to find the correct position, add block to free list and update order
    while (current) {
        if (current > block_to_free) {
            // block_to_free->next = current;  // attach block to free to next
            // if(prev) {
            //     prev->next = block_to_free;     // attach previous to block to free
            // }
            break;
        }
        prev = current;
        current = current->next;
    }

    // Insert the block back into the free list (sorted order)
    if (prev) { // middle or end of list

        int right = coalesce_right(current,block_to_free);
        if (right)
        {
            prev->next = block_to_free;
            current = block_to_free->next;
        }
        int left =  coalesce_left(prev, block_to_free);
        if (left)
        {
            prev->next = current;
        }
        if (!right && ! left)
        {
            prev->next = block_to_free;
            block_to_free->next = current;
        }
    }
    else { // head of the list
        // coallece right?
        if (!coalesce_right(current,block_to_free)) {
            //block_to_free->next = free_list;
            block_to_free->next = current;
        }
        //block_to_free->next = current;
        free_list = block_to_free;
    }
}
/**
 * Frees the memory pointed to by ptr.
 * If the pointer is NULL, prints an error message and returns.
 * Performs backward and forward coalescing to merge adjacent free blocks.
 *
 * @param ptr A pointer to the memory to be freed.
 */
void magic_free(void* ptr) {
    if (!ptr) {
        printf("Error: Attempted to free Null pointer\n");
        return;
    }

    Block* block_to_free = (Block*)ptr - 1; // Block pointer
    if (block_to_free->free) {
        printf("Error: Memory Requested to free is already free\n");
        return;
    }
    block_to_free->free = 1;

    if (free_list == NULL) {
        free_list = block_to_free;
        free_list->next = NULL;
        return;
    }

    insert_block(block_to_free);

    //ptr = NULL;
}


Block* find_free_block(size_t size) {
    Block* current = free_list;
    while (current && (current->size < size)){
        current = current->next;
    }
    return current; // NULL if no block is found
}

void split_free_block(Block *current, size_t size){
    Block* new_block = (Block*)((char*)current + size + BLOCK_SIZE);
    new_block->size = current->size - size - BLOCK_SIZE; 
    new_block->free = 1;
    new_block->next = current->next;

    current->size = size;
    current->free = 0;
    current->next = new_block;
}
/**
 * Allocates memory of the specified size.
 * If the allocation is successful, returns a pointer to the allocated memory.
 * If the allocation fails, prints an error message and returns NULL.
 *
 * @param size The size of the memory to allocate.
 * @return A pointer to the allocated memory or NULL if the allocation fails.
 */
void* magic_malloc(size_t size) {
    if (size <= 0 || size > MEMORY_POOL_SIZE - BLOCK_SIZE) {
        printf("Error: Allocated invalid number of Bytes\n");
        return NULL;
    }

    size = ALIGN(size);
    Block* prev = NULL;
    Block* current = free_list;

    while (current && (current->size < size)) {     // Find a free block and keep track of the previous node
        prev = current;
        current = current->next;
    }
    
    if (!current) {
        printf("Error: Memory allocation of %zu bytes failed. No suitable free block found.\n", size);
        return NULL;
    }

    if (current->size > size + BLOCK_SIZE) {// split block and create new free block
        split_free_block(current,size);
    }
    else {// available memory is exact size of requrested or not enough for a new block 
        current->free = 0;
    }

    // remove allocated block from free_list
    if (prev) {
        prev->next = current->next;
    } 
    else {
        free_list = current->next;
    }
    return(void*)(current + 1);
}

void* magic_calloc(size_t num, size_t size)
{
    size_t total_size = num * size;
    void* ptr = magic_malloc(total_size);
    if (ptr) memset(ptr, 0, total_size);    
    return ptr;
}

void* magic_realloc(void* ptr, size_t new_size)
{
    if (!ptr) return magic_malloc(new_size);

    Block* current_block = (Block*)ptr - 1; // Block pointer
    if (current_block->free) {
        return magic_malloc(new_size);
    }

    if (new_size <= 0) {
        magic_free(ptr);
        return NULL;
    }

    size_t current_size = current_block->size;
    if (new_size <= current_size) return ptr;

    // Block* next_block = current_block->next;
    // if (next_block && next_block->free && (current_size + sizeof(Block) + next_block->size) >= new_size) {  // check if right adjacent block can accomodate new size
    //     // Merge the current block with the next free block
    //     current_block->size += sizeof(Block) + next_block->size;
    //     current_block->next = next_block->next;
    //     return ptr;  // No need to copy, memory is already expanded
    // }

    magic_free(ptr);
    void* new_ptr = magic_malloc(new_size);
    memcpy(new_ptr,ptr,new_size);
    return new_ptr;
}

/**
 * Prints the current state of the memory pool.
 * Displays metadata and whether each block is allocated or free.
 */
void visualize_memory_pool()
{
    Block* current = (Block*)memory_pool;
    int blockIndex = 0;

    printf("\n[Memory Pool Visualization]\n");
    printf("Pool Size: %d bytes\n\n", MEMORY_POOL_SIZE);

    while ((char*)current < memory_pool + MEMORY_POOL_SIZE) {
        // Masking the address to fit into 9 digits
        uintptr_t startAddress = (uintptr_t)current % 1000000000;
        uintptr_t dataStart = ((uintptr_t)current + BLOCK_SIZE) % 1000000000;
        uintptr_t dataEnd = (dataStart + current->size - 1) % 1000000000;
        //uintptr_t nextAddress = current->next ? ((uintptr_t)current->next % 1000000000) : 0;

        printf("Block %d:\n", blockIndex++);
        printf("  Start Address: %09lu\n", startAddress);
        printf("  Block Size: %zu bytes\n", current->size + BLOCK_SIZE);
        printf("  Allocated: %s\n", current->free ? "NO (Free)" : "YES (Allocated)");
        printf("  Data Range: [%09lu - %09lu] (%zu bytes)\n", dataStart, dataEnd, current->size);
        ///printf("  Next Block Address: %09lu\n", nextAddress);

        // Move to the next block
        current = (Block*)((char*)current + BLOCK_SIZE + current->size);
        printf("\n");
    }
}

int main () 
{
    run_all_tests();
    run_coalesing_tests();
    run_performace_tests();

    return 0;
}