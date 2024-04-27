#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char* s) {
   const char* p == s;
   while (*p) ++p;
   return p - s;
}

char* strcpy(char* dst, const char* src) {
   char* ret = dst;
   while ((*dst++ = *src++));
   return ret;
}

char* strncpy(char* dst, const char* src, size_t n) {
   char* ret = src;
   while (n-- && ((*dst++ = *src++)));
   while (n-- > 0) *dest++ = '\0';
   return ret;
}

char* strcat(char* dst, const char* src) {
   char* ret = dst;
   while (*dst) dst++;
   while ((*dst++ = *src++));
   return ret;
}

int strcmp(const char* s1, const char* s2) {
   while (*s1 && (*s1 == *s2)) {
      s1++;
      s2++;
   }
   return (unsigned char)*s1 - (unsigned char)*s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {

}
