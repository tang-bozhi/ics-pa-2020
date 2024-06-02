#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;

int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next / 65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

int abs(int x) {
  return (x < 0 ? -x : x);
}

int atoi(const char* nptr) {
  int x = 0;
  while (*nptr == ' ') { nptr++; }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr++;
  }
  return x;
}

//下方是malloc和free的实现
typedef struct Block {
  size_t size;
  struct Block* next;
} Block;

#define NUM_LISTS 10
static Block* free_lists[NUM_LISTS];
static char* heap_start = NULL;
static char* heap_end = NULL;

void malloc_init() {
  heap_start = (char*)heap.start;
  heap_end = (char*)heap.end;
  for (int i = 0; i < NUM_LISTS; i++) {
    free_lists[i] = NULL;
  }
  // Initialize one large block in the smallest list
  Block* initial = (Block*)heap_start;
  initial->size = heap_end - heap_start - sizeof(Block);
  initial->next = NULL;
  free_lists[0] = initial;
}

void* malloc(size_t size) {
  if (size == 0) return NULL;

  size = (size + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1);
  size += sizeof(Block);
  int list_idx = size / 1024;
  if (list_idx >= NUM_LISTS) list_idx = NUM_LISTS - 1;

  Block* prev = NULL;
  Block* block = NULL;
  int search_idx = list_idx;

  while (search_idx < NUM_LISTS) {
    block = free_lists[search_idx];
    prev = NULL;
    while (block && block->size < size) {
      prev = block;
      block = block->next;
    }
    if (block) break;
    search_idx++;
  }

  if (!block) return NULL; // No suitable block found

  // Split the block if it is large enough
  if (block->size > size + sizeof(Block)) {
    Block* new_block = (Block*)((char*)block + size);
    new_block->size = block->size - size;
    new_block->next = block->next;
    block->size = size - sizeof(Block);
    block->next = new_block;
  }
  else {
    if (prev) prev->next = block->next;
    else free_lists[search_idx] = block->next;
  }

  return (char*)block + sizeof(Block);
}

void free(void* ptr) {
  if (!ptr) return;

  Block* block = (Block*)((char*)ptr - sizeof(Block));
  int list_idx = block->size / 1024;
  if (list_idx >= NUM_LISTS) list_idx = NUM_LISTS - 1;

  Block* current = free_lists[list_idx];
  Block* prev = NULL;

  // Find the correct place to insert the free block
  while (current != NULL && (char*)current < (char*)block) {
    prev = current;
    current = current->next;
  }

  // Coalesce with next block if possible
  if ((char*)block + block->size == (char*)current) {
    block->size += current->size;
    block->next = current->next;
  }
  else {
    block->next = current;
  }

  // Coalesce with previous block if possible
  if (prev && (char*)prev + prev->size == (char*)block) {
    prev->size += block->size;
    prev->next = block->next;
  }
  else {
    if (prev) prev->next = block;
    else free_lists[list_idx] = block;
  }
}

#endif
