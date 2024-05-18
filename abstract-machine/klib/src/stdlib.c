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
#define HEAP_SIZE 1024*1024  // 定义堆大小为1MB
static char heap[HEAP_SIZE];  // 堆空间
static void* free_list = NULL;  // 空闲块链表

typedef struct Block {
  size_t size;  // 数据块大小
  struct Block* next;  // 指向下一个块的指针
} Block;

void initialize_memory() {
  free_list = (void*)heap;  // 初始化空闲链表指向堆的开始
  ((Block*)free_list)->size = HEAP_SIZE - sizeof(Block);  // 初始化第一个块的大小
  ((Block*)free_list)->next = NULL;  // 没有下一个块
}

void* malloc(size_t size) {
  Block* current = (Block*)free_list;
  Block* previous = NULL;

  while (current != NULL) {
    if (current->size >= size + sizeof(Block)) {  // 检查块的大小是否足够
      if (current->size == size + sizeof(Block)) {  // 完美匹配
        if (previous == NULL) {  // 更新头部
          free_list = current->next;
        }
        else {
          previous->next = current->next;
        }
        return (void*)(current + 1);  // 返回数据区
      }
      else {  // 分割块
        size_t remaining_size = current->size - size - sizeof(Block);
        current->size = size;
        Block* new_block = (Block*)((char*)(current + 1) + size);
        new_block->size = remaining_size;
        new_block->next = current->next;
        if (previous == NULL) {
          free_list = new_block;
        }
        else {
          previous->next = new_block;
        }
        return (void*)(current + 1);
      }
    }
    previous = current;
    current = current->next;
  }

  return NULL;  // 没有足够的空间
}

void free(void* ptr) {
  Block* block = (Block*)ptr - 1;  // 获取控制块(1指的是从指针 `ptr` 中减去一个 `Block` 结构的大小。)
  block->next = (Block*)free_list;
  free_list = block;
}

#endif
