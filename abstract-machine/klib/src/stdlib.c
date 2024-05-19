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

static char* addr;  // 用来追踪堆中当前位置的指针

void* malloc(size_t size) {
  if (addr + size > (char*)heap.end) {  // 检查是否有足够的空间
    return NULL;  // 空间不足
  }

  char* ret = addr;  // 保存当前地址以返回
  addr += size;  // 通过分配块的大小增加 addr
  return ret;  // 返回之前的当前地址
}



void free(void* ptr) {
  // 这个函数故意留空
  // 通过这个 malloc 分配的内存不能被释放
}


#endif
