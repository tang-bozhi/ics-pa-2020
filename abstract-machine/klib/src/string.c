#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char* s) {
  const char* p = s;
  while (*p) ++p;
  return p - s;
}

char* strcpy(char* dst, const char* src) {
  char* ret = dst;
  while ((*dst++ = *src++));
  return ret;
}

char* strncpy(char* dst, const char* src, size_t n) {
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
  unsigned char* p = v;
  while (n--) *p++ = c;
  return v;
}

void* memmove(void* dst, const void* src, size_t n) {
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
