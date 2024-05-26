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
// 用来追踪当前分配位置的指针
static char* addr;

void malloc_init() {
  // 初始化 addr 为堆的起始地址
  addr = (char*)heap.start;
}

void* malloc(size_t size) {
  // 检查是否有足够的内存分配请求的大小
  if (addr + size > (char*)heap.end) {
    return NULL;  // 没有足够空间，返回 NULL
  }

  char* ret = addr;  // 保存当前地址以返回
  addr += size;  // 通过分配块的大小增加 addr
  return ret;  // 返回已分配内存的起始地址
}

void free(void* ptr) {
  // 根据指示留空
}

#endif
