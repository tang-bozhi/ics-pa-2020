#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char* s) {//计算字符串 str 的长度，直到空结束字符，但不包括空结束字符。
  const char* p = s;
  while (*p) ++p;
  return p - s;
}

char* strcpy(char* dst, const char* src) {//把 src 所指向的字符串复制到 dest. 需要注意的是如果目标数组 dest 不够大，而源字符串的长度又太长，可能会造成缓冲溢出的情况。
  char* ret = dst;
  while ((*dst++ = *src++));
  return ret;
}

char* strncpy(char* dst, const char* src, size_t n) {//把 src 所指向的字符串复制到 dest，最多复制 n 个字符。当 src 的长度小于 n 时，dest 的剩余部分将用空字节填充。
  char* ret = dst;
  while (n-- && (*dst++ = *src++)); // Copy until n chars or null byte is copied
  while (n-- > 0) *dst++ = '\0'; // Pad with null bytes if n > strlen(src)
  return ret;
}

char* strcat(char* dst, const char* src) {
  char* ret = dst;
  while (*dst) dst++; // Find end of dst
  while ((*dst++ = *src++)); // Copy src to the end of dst
  return ret;
}

int strcmp(const char* s1, const char* s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  while (n-- && *s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return n == (size_t)-1 ? 0 : *(unsigned char*)s1 - *(unsigned char*)s2;
}

void* memset(void* v, int c, size_t n) {
  unsigned char* p = v;//unsigned char*无符号字符的指针: 为了能逐字节地操作内存
  while (n--) *p++ = c;
  return v;
}

void* memmove(void* dst, const void* src, size_t n) {//C 库函数 void *memmove(void *str1, const void *str2, size_t n) 从 str2 复制 n 个字符到 str1，但是在重叠内存块这方面，memmove() 是比 memcpy() 更安全的方法。如果目标区域和源区域有重叠的话，memmove() 能够保证源串在被覆盖之前将重叠区域的字节拷贝到目标区域中，复制后源区域的内容会被更改。如果目标区域与源区域没有重叠，则和 memcpy() 函数功能相同。
  char* pd = dst;
  const char* ps = src;
  if (ps < pd && pd < ps + n) {
    pd += n;
    ps += n;
    while (n--) *--pd = *--ps;
  }
  else {
    while (n--) *pd++ = *ps++;
  }
  return dst;
}

void* memcpy(void* out, const void* in, size_t n) {
  char* dest = out;
  const char* src = in;
  while (n--) *dest++ = *src++;
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n) {
  const unsigned char* p1 = s1, * p2 = s2;
  while (n--) {
    if (*p1 != *p2) {
      return *p1 - *p2;
    }
    p1++;
    p2++;
  }
  return 0;
}

#endif
