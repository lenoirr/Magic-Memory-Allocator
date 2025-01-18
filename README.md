# Magic Memory Allocator

## Overview

The **Magic Memory Allocator** is a custom dynamic memory allocation implementation in C that uses a simulated heap and a linked list to manage memory blocks. It provides `magic_malloc` for memory allocation, `magic_free` for freeing memory, and utilities for visualization and testing. This project demonstrates techniques like memory splitting, coalescing, and safe memory management practices.

---

## Motivation

The **Magic Memory Allocator** was created as part of a project to deepen my understanding of memory management concepts critical in systems programming and embedded development. While working on this project, I wanted to explore the inner workings of dynamic memory allocation—something we often take for granted when using built-in functions like `malloc` and `free`.

I embarked on this journey to:
- **Demystify The Magic of Low-Level Concepts**: Gain firsthand experience in managing memory with raw pointers, alignment considerations, and fragmentation reduction.
- **Improve Problem-Solving Skills**: Tackle challenges such as splitting blocks, coalescing free memory, and designing efficient algorithms for allocation and deallocation.
- **Prepare for Real-World Applications**: Strengthen my foundation for working on projects where constrained memory environments, such as embedded systems, demand custom memory solutions.

---


## Features

1. **Dynamic Memory Allocation (`magic_malloc`)**:
   - Allocates memory blocks from a fixed memory pool.
   - Aligns memory sizes for optimal performance.
   - Splits larger blocks if necessary for efficient space utilization.

2. **Memory Deallocation (`magic_free`)**:
   - Frees allocated memory blocks.
   - Coalesces adjacent free blocks to reduce fragmentation.
   - Prevents double-free and invalid-pointer errors.

3. **Memory Visualization**:
   - Displays the current state of the memory pool, including block metadata (size, state, addresses).

4. **Unit Tests**:
   - Comprehensive tests for allocation, deallocation, and edge cases like zero-byte or over-size allocation.

---

## Constants
- `MEMORY_POOL_SIZE`: Total memory pool size (1024 bytes).
- `BLOCK_SIZE`: Size of the `Block` metadata structure.

---

## Core Data Structures
```c
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
```
### `memory_pool`
`char memory_pool[MEMORY_POOL_SIZE:` A contiguous memory array simulating the heap.

### `Block`
Represents a block of memory in the pool, including metadata:
- `size`: Size of the memory block (excluding metadata).
- `free`: Indicates whether the block is free (`1`) or allocated (`0`).
- `next`: Pointer to the next block in the free list.

### `free_list`
`Block* free_list` Singly linked list to manage free blocks of varying size dynamically.
- This structure allows the allocator to easily split and coalesce blocks to reduce fragmentation and increase memory utilization.

---

## Key Functions

### Initialization
- Before using the allocator, initialize the memory pool;
```c
initialize_memory_pool();
```
### Allocating Memory
- Use `magic_malloc` to allocate memory.
```c
void* ptr = magic_malloc(size_t size);
```
### Freeing Memory
- Use `magic_free` to free allocated memory.
```c
magic_free(ptr);
```
### Visualizing the Memory Pool
- Display the current memory pool state for debugging.
```c
visualize_memory_pool();
```
#### Sample Output
- Example output of `visualize_memory_pool();`
  
![image](https://github.com/user-attachments/assets/fd0b78b4-fc64-4836-80cf-3216ef5cb9e5)

---

## Testing

The code includes test functions to verify the implementation:
- `test_allocate_full_pool()`
- `test_free_middle_block()`
- `test_allocate_zero_bytes()`
- `test_allocate_larger_than_pool()`
- `test_allocate_and_free_all()`
- `test_fragmentation_and_coalescing()`
- `test_double_free()`
To run all tests:
```c
int main() {
    run_all_tests();
    return 0;
}
```
Each test uses `assert` and `visualize_memory_pool` to validate functionality, print error details, and help you visualize the memory structure.

### Sample Test Output
- Example output when passing `test_allocate_full_pool()`

![image](https://github.com/user-attachments/assets/4f2c365c-7be9-49a8-ad40-7003b488c18f)

---

## Lessons Learned

1. **Understanding Allocation Mechanics**: I gained insight into how metadata, alignment, and splitting work behind the scenes of memory allocators.
   
2. **Managing Fragmentation**: Learned techniques for reducing memory fragmentation through block coalescing.
   
3. **Debugging with Visualization**: Creating a Visualization of memory pools helped identify bugs and understand allocation behavior more intuitively.
   
4. **Importance of Edge Case Handling**: Ensured reliability by writing test cases for zero-byte allocations, oversized requests, and scenarios.


---

  
