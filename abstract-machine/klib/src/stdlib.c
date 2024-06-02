#include <am.h>
#include <klib.h>
#include <klib-macros.h>

// 仅当非本地 ISA 或启用了本地 klib 时才编译以下代码
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;

// 生成伪随机数，返回值在 [0, 32767] 范围内
int rand(void) {
  // RAND_MAX 假设为 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next / 65536) % 32768;
}

// 设置随机数生成器的种子
void srand(unsigned int seed) {
  next = seed;
}

// 返回整数的绝对值
int abs(int x) {
  return (x < 0 ? -x : x);
}

// 将字符串转换为整数
int atoi(const char* nptr) {
  int x = 0;
  // 跳过前导空格
  while (*nptr == ' ') { nptr++; }
  // 转换每个字符为数字并累加
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr++;
  }
  return x;
}

// 下面是 malloc 和 free 的实现

typedef struct Block {
  size_t size;          // 块大小
  struct Block* next;   // 指向下一个块的指针
} Block;

#define NUM_LISTS 10
static Block* free_lists[NUM_LISTS];   // 空闲块链表数组
static char* heap_start = NULL;        // 堆的起始地址
static char* heap_end = NULL;          // 堆的结束地址

// 初始化 malloc 系统
void malloc_init() {
  heap_start = (char*)heap.start;
  heap_end = (char*)heap.end;
  for (int i = 0; i < NUM_LISTS; i++) {
    free_lists[i] = NULL;
  }
  // 在最小的链表中初始化一个大块
  Block* initial = (Block*)heap_start;
  initial->size = heap_end - heap_start - sizeof(Block);
  initial->next = NULL;
  free_lists[0] = initial;
}

// 分配指定大小的内存
void* malloc(size_t size) {
  if (size == 0) return NULL;

  // 调整大小以包含块头，并对齐到 sizeof(size_t) 的倍数
  size = (size + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1);
  size += sizeof(Block);
  int list_idx = size / 1024;
  if (list_idx >= NUM_LISTS) list_idx = NUM_LISTS - 1;

  Block* prev = NULL;
  Block* block = NULL;
  int search_idx = list_idx;

  // 从适当的链表中查找足够大的块
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

  // 未找到合适的块
  if (!block) return NULL;

  // 如果块足够大，则拆分块
  if (block->size > size + sizeof(Block)) {
    Block* new_block = (Block*)((char*)block + size);
    new_block->size = block->size - size;
    new_block->next = block->next;
    block->size = size - sizeof(Block);
    block->next = new_block;
  }
  else {
    // 否则从链表中移除块
    if (prev) prev->next = block->next;
    else free_lists[search_idx] = block->next;
  }

  return (char*)block + sizeof(Block);
}

// 释放已分配的内存
void free(void* ptr) {
  if (!ptr) return;

  Block* block = (Block*)((char*)ptr - sizeof(Block));
  int list_idx = block->size / 1024;
  if (list_idx >= NUM_LISTS) list_idx = NUM_LISTS - 1;

  Block* current = free_lists[list_idx];
  Block* prev = NULL;

  // 找到正确的位置插入空闲块
  while (current != NULL && (char*)current < (char*)block) {
    prev = current;
    current = current->next;
  }

  // 如果可能，与下一个块合并
  if ((char*)block + block->size == (char*)current) {
    block->size += current->size;
    block->next = current->next;
  }
  else {
    block->next = current;
  }

  // 如果可能，与前一个块合并
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
