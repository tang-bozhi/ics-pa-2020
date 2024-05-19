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
// 定义空闲块结构
typedef struct FreeBlock {
  size_t size;
  struct FreeBlock* next;
} FreeBlock;

static FreeBlock* free_list;
static char* heap_start;
static char* heap_end;

// 初始化堆
void heap_init(Area heap) {
  heap_start = (char*)heap.start;
  heap_end = (char*)heap.end;
  free_list = (FreeBlock*)heap_start;
  free_list->size = heap_end - heap_start - sizeof(FreeBlock);
  free_list->next = NULL;
}

void* malloc(size_t size) {
  if (size == 0) return NULL;

  FreeBlock** best_fit = NULL;
  FreeBlock** curr = &free_list;

  // 找到最适合的空闲块
  while (*curr) {
    if ((*curr)->size >= size) {
      if (!best_fit || (*curr)->size < (*best_fit)->size) {
        best_fit = curr;
      }
    }
    curr = &(*curr)->next;
  }

  if (!best_fit) {
    printf("Error allocating memory! Requested size: %zu\n", size);  // Debugging output
    return NULL;  // 找不到合适的块
  }

  FreeBlock* block = *best_fit;
  if (block->size >= size + sizeof(FreeBlock) + 1) {
    // 如果块足够大，则拆分
    char* allocated_memory = (char*)block + sizeof(FreeBlock);
    size_t remaining_size = block->size - size - sizeof(FreeBlock);
    FreeBlock* new_block = (FreeBlock*)(allocated_memory + size);
    new_block->size = remaining_size;
    new_block->next = block->next;
    *best_fit = new_block;
  }
  else {
    *best_fit = block->next;
  }

  return (char*)block + sizeof(FreeBlock);
}

void free(void* ptr) {
  if (!ptr) return;

  if ((char*)ptr < heap_start || (char*)ptr >= heap_end) {
    printf("Error: trying to free memory outside the heap range!\n");  // Debugging output
    return;
  }

  FreeBlock* block_to_free = (FreeBlock*)((char*)ptr - sizeof(FreeBlock));
  FreeBlock** curr = &free_list;

  // 按地址顺序插入释放的块
  while (*curr && *curr < block_to_free) {
    curr = &(*curr)->next;
  }
  block_to_free->next = *curr;
  *curr = block_to_free;

  // 合并相邻的空闲块
  curr = &free_list;
  while (*curr && (*curr)->next) {
    if ((char*)(*curr) + (*curr)->size + sizeof(FreeBlock) == (char*)(*curr)->next) {
      (*curr)->size += (*curr)->next->size + sizeof(FreeBlock);
      (*curr)->next = (*curr)->next->next;
    }
    else {
      curr = &(*curr)->next;
    }
  }
}



#endif
