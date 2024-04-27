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

char* strncpy(char* dst, const char* src) {
   char* ret = src;
   while (n-- && ((*dst++ = *src++)));
   while (n-- > 0) *dest++ = '\0';
   return ret;
}

char* strcat(char* dst, const char* src) {
   char*
}
