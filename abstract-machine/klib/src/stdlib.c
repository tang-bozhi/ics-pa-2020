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

static Block* free_list = NULL;
static char* heap_start;
static char* heap_end;

void malloc_init() {
  heap_start = (char*)heap.start;
  heap_end = (char*)heap.end;
  free_list = (Block*)heap_start;
  free_list->size = heap_end - heap_start - sizeof(Block);
  free_list->next = NULL;
}

void* malloc(size_t size) {
  Block* block = free_list;
  Block* prev = NULL;

  size = (size + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1);  // Align size to size_t
  size += sizeof(Block);  // Add size for the header

  while (block != NULL) {
    if (block->size >= size) {
      if (block->size > size + sizeof(Block)) {
        // Split block
        Block* new_block = (Block*)((char*)block + size);
        new_block->size = block->size - size;
        new_block->next = block->next;
        block->size = size - sizeof(Block);
        block->next = new_block;
      }
      else {
        // Block is perfectly sized or too small to split
        if (prev == NULL) {
          free_list = block->next;
        }
        else {
          prev->next = block->next;
        }
      }

      return (char*)block + sizeof(Block);
    }
    prev = block;
    block = block->next;
  }

  return NULL;  // No suitable block found
}

void free(void* ptr) {
  if (ptr == NULL) return;

  Block* block = (Block*)((char*)ptr - sizeof(Block));
  block->next = free_list;
  free_list = block;

  // Optional: Coalesce adjacent free blocks
}

#endif
