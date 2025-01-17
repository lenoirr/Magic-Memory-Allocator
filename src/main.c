#include <stdio.h>
#include <stdlib.h>

#define ALIGNMENT 8
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))  // Ensures size is a multiple of the alignment

#define MAX_SIGNED_32_BIT 2147483647
// Linked List Node Declaration - Meta Data about a block of memory. 
typedef struct Block 
{
    
    size_t size;            // size of block                        
    int free;               // 1 if the block is allocated          // NOTE: Padding to 8 Bytes on this Int will result in 4 Bytes of wasted pace per allocation.       
    struct Block *next;     // pointer the next block of memory.    
    
} Block;

#define MEMORY_POOL_SIZE 1024 // 1KB
#define BLOCK_SIZE sizeof(struct Block)

static char memory_pool[MEMORY_POOL_SIZE];          // Simulated Heap
static Block *free_list = (Block*)memory_pool;      // linked list to track memory allocation - block created at the start of the memory pool

// Initializing the first block of metaData. 
void initialize_memory_pool()
{
    free_list->size = MEMORY_POOL_SIZE - BLOCK_SIZE;    
    free_list->next = NULL;                              
    free_list->free = 1;                                
}
/**
 * Frees the memory pointed to by ptr.
 * If the pointer is NULL, prints an error message and returns.
 * Performs backward and forward coalescing to merge adjacent free blocks.
 *
 * @param ptr A pointer to the memory to be freed.
 */
void magic_free(void* ptr)
{
    if (!ptr)
    {
        printf("Error: Attempted to free Null pointer\n");
        return;
    }

    Block* current = free_list;
    Block* previous = NULL;

    while(current)
    {
        if(ptr == (current + 1))
        {
            current->free = 1;

            // Backward coalescing 
            if (previous && previous->free)
            {
                previous->size += BLOCK_SIZE + current->size;
                previous->next = current->next;
                current = previous;
            }

            // Forward coalescing
            if (current->next && current->next->free)
            {
                current->size += BLOCK_SIZE + current->next->size;
                current->next = current->next->next;
            }

            return;
        }

        previous = current;
        current = current->next;
    }
    printf("Error: Attempted to free invalid pointer\n");

}

/**
 * Allocates memory of the specified size.
 * If the allocation is successful, returns a pointer to the allocated memory.
 * If the allocation fails, prints an error message and returns NULL.
 *
 * @param size The size of the memory to allocate.
 * @return A pointer to the allocated memory or NULL if the allocation fails.
 */
void* magic_malloc(size_t size)
{
     if (size <= 0)
     {
        printf("Error: Allocated invalid number of Bytes\n");
        return NULL;
     }

     Block* current = free_list;
     while(current) // step through free list
     {
        if ((current->size >= size + BLOCK_SIZE) && current->free)
        {
            current->free = 0;
            Block *new_block = (Block*)((char*)current + BLOCK_SIZE + size);           
            if ((current->size > size + BLOCK_SIZE))
            {           
                new_block->size = current->size - size - BLOCK_SIZE;
                new_block-> free = 1;
                new_block->next = current->next;
                current->size = size;
                current->next = new_block;           
            }
            else 
            {
                new_block->free = 0;
                current->next = new_block; 
            }

            return (void*)(current + 1);    // return void pointer to one block structure away.
        }
        current = current->next;
     }
     return NULL; // no blocks are available
}

/**
 * Prints the current state of the memory pool.
 * Displays metadata and whether each block is allocated or free.
 */
void visualize_memory_pool()
{
    Block* current = free_list;
    int MetaDataStartElement = 0;
    printf("\n");

    while(current)
    {
        int MetaDataEndElement = MetaDataStartElement + BLOCK_SIZE - 1;
        printf("[META-DATA]:[%d]-[%d]--",MetaDataStartElement,MetaDataEndElement);
        int BlockEndElement = MetaDataEndElement + current->size;

        if(!current->free)
        {
            printf("[Allocated-DATA: SIZE(%d)]:[%d]-[%d].",current->size,MetaDataEndElement + 1, BlockEndElement);            
        }
        else 
        {
            printf("[FREE-DATA: SIZE(%d)]:[%d]-[%d].",current->size,MetaDataEndElement + 1, BlockEndElement);
        }
        MetaDataStartElement = ++BlockEndElement;
        current = current->next;
    }
}

int main ()
{
    
    initialize_memory_pool();

    //int *pInt = (int*)magic_malloc(MEMORY_POOL_SIZE - BLOCK_SIZE - BLOCK_SIZE);
    int *pInt1 = (int*)magic_malloc(25*sizeof(int));
    if (pInt1 == NULL)
    {
        printf("Integert Memory Allocation Failed");
    }
    visualize_memory_pool();

    int *pInt2 = (int*)magic_malloc(25*sizeof(int));
    if (pInt2 == NULL)
    {
        printf("Integert Memory Allocation Failed");
    }
    visualize_memory_pool();

    
    int *pInt3 = (int*)magic_malloc(25*sizeof(int));
    if (pInt3 == NULL)
    {
        printf("Integert Memory Allocation Failed");
    }
    visualize_memory_pool();

    int *pInt4 = (int*)magic_malloc(25*sizeof(int));
    if (pInt4 == NULL)
    {
        printf("Integert Memory Allocation Failed");
    }
    visualize_memory_pool();

    magic_free(pInt1);
    visualize_memory_pool();

    magic_free(pInt3);
    visualize_memory_pool();

    magic_free(pInt2);
    visualize_memory_pool();

    int *pInt5 = (int*)magic_malloc(50*sizeof(int));
    if (pInt5 == NULL)
    {
        printf("Integert Memory Allocation Failed");
    }
    visualize_memory_pool();

    int *pInt6 = (int*)magic_malloc(300);
    if (pInt6 == NULL)
    {
        printf("Integert Memory Allocation Failed");
    }
    visualize_memory_pool();

/*
    short *pShortarr = (short*)magic_malloc(3*sizeof(short));

    if (pShortarr == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }

    for (int i = 0; i < 3; i++) {
        pShortarr[i] = (short)(i * 10);  // Example values: 0, 10, 20
    }
*/
    
    return 0;
}