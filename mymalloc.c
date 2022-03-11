#define _DEFAULT_SOURCE
#define _BSD_SOURCE 
#include <malloc.h> 
#include <stdio.h>  // Any other headers we need here.
// NOTE: You should NOT include <stdlib.h> in your final implementation.
#include <string.h>
#include <unistd.h>

#include <debug.h> // definition of debug_printf.

#define BLOCK_SIZE sizeof(block_t)

// Construct a block that contains the current block's size, its next element and whether it is free. 
typedef struct block {
  size_t size;        // How many bytes beyond this block have been allocated in the heap.
  struct block *next; // Where is the next block in your linked list.
  int free;           // Is this memory free, i.e., available to give away?
} block_t;

// Implement the malloc function using sbrk(). 
// Set up the first block where entering the program (each progam will call the first block first if it is NULL).
// It will be NULL for every new program (not include each malloc in a single program). 
block_t *first_block;
void *mymalloc(size_t s) {
  // This situation is the first block needed to be allocated in the program. 
  if (first_block == NULL) { 
    // Allocated size needs to be the required size plus the size of the block. 
    block_t *first_block = sbrk(s + BLOCK_SIZE);
    // Check whether the sbrk call or memory allocation is succeed or not. 
    if (first_block == (void *) -1) {
      return NULL;
    }
    first_block->size = s;
    first_block->next = NULL;
    first_block->free = 1;
    // Print how much memory are allocated. 
    debug_printf("malloc %zu bytes\n", s);
    // Return the address of the first block + 1 as the allocated memory location. 
    return (void *) (first_block + 1);
  } 
  // Not the first block, we need to search for each individual block after the first block. 
  // Temporary block holder for each block search after the first blok. 
  block_t *current_block = first_block;
  // Start searching using a while true loop, when a available block appear.   
  while (1) {
    // Situation 1: The block is free and the available size is larger than the given required size. 
    // Give this block to the required memory allocation and reset the block size as the given required size. 
    // No need to create a new block since the old created block that is free can be used. 
    if (current_block->free == 0 && current_block->size >= s) {
      current_block->size = s;
      current_block->next = current_block->next;
      current_block->free = 1;
      // Print how much memory are allocated. 
      debug_printf("malloc %zu bytes\n", s);
      // Return the address of the current block + 1 as the allocated memory location. 
      return (void *) (current_block + 1);
    } else if (current_block->next == NULL) {
      // Situation 2: The block's next block is null, which means that this block is currently the last block in the linked list. 
      // This means that all previous blocks are not available and needs to create a block to extend the memory linked list. 
      // This newly created block will be set as the next block of the current block. 
      // Allocated size needs to be the required size plus the size of the block. 
      current_block->next = sbrk(s + sizeof(block_t));
      // Check whether the sbrk call or memory allocation is succeed or not. 
      if (current_block->next == (void *) -1) {
        return NULL;
      }
      current_block->next->size = s;
      current_block->next->next = NULL;
      current_block->next->free = 1;
      // Print how much memory are allocated. 
      debug_printf("malloc %zu bytes\n", s);
      // Return the address of the first block + 1 as the allocated memory location. 
      return (void *) (current_block->next + 1);;
    }

    // Current block cannot be allocated for the given block.
    current_block = current_block->next;
  }
  return NULL;
}

// Implement calloc function using malloc and set the all allocated memory as 0 as calloc required.  
void *mycalloc(size_t nmemb, size_t s) {
  // Use malloc to allocate memory space for the given required memory space. 
  void * request = malloc(s * nmemb);
  // Set all memory in the given required memory space as value of 0. 
  memset(request, 0, nmemb * s);
  debug_printf("calloc %zu bytes\n", s);
  // Return the block location in the memory. 
  return request;
}

// Implement free function with set the block's free status as free. 
void myfree(void *ptr) {
  // Retrieve the pointer location's block and get the size. 
  size_t free_size = (((block_t *) ptr) - 1)->size;
  // Return the block free in the memory. 
  debug_printf("Freed %zu bytes\n", free_size);
  // Set the block's free status in this pointer location as free. 
  (((block_t *) ptr) - 1) ->free = 0; 
}
