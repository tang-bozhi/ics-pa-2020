#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// 计算字符串`s`的长度，不包括结尾的空字符。
size_t strlen(const char* s) {
   const char* p = s;
   while (*p) ++p; // 指针增加至空字符
   return p - s;   // 返回指针之差，即长度
}

// 将字符串`src`复制到`dst`。目标字符串必须有足够的空间来保存`src`。
char* strcpy(char* dst, const char* src) {
   char* ret = dst;
   while ((*dst++ = *src++)); // 复制字符直到包括空字符
   return ret;
}

// 最多复制`n`个字符从字符串`src`到`dst`。如果`src`的长度小于`n`，`dst`的剩余部分将用`\0`填充。
char* strncpy(char* dst, const char* src, size_t n) {
   char* ret = dst;
   while (n-- && (*dst++ = *src++)); // 复制直到达到n个字符或复制了空字符
   while (n-- > 0) *dst++ = '\0';    // 如果n大于src的长度，用空字节填充
   return ret;
}

// 将字符串`src`追加到`dst`的末尾。假设`dst`有足够的空间来保存结果。
char* strcat(char* dst, const char* src) {
   char* ret = dst;
   while (*dst) dst++;          // 找到dst的末尾
   while ((*dst++ = *src++));   // 将src复制到dst的末尾
   return ret;
}

// 比较两个字符串`s1`和`s2`。如果相等返回0，如果`s1 < s2`返回负值，如果`s1 > s2`返回正值。
int strcmp(const char* s1, const char* s2) {
   while (*s1 && (*s1 == *s2)) {
      s1++;
      s2++;
   }
   return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// 比较两个字符串`s1`和`s2`的前`n`个字符。
// 如果相等或`n`为零，返回0。如果在比较`n`个字符之前`s1 < s2`，返回负值；如果`s1 > s2`，返回正值。
int strncmp(const char* s1, const char* s2, size_t n) {
   while (n-- && *s1 && (*s1 == *s2)) {
      s1++;
      s2++;
   }
   return n == (size_t)-1 ? 0 : *(unsigned char*)s1 - *(unsigned char*)s2;
}

// 将从`v`开始的`n`个字节设定为字节`c`。
void* memset(void* v, int c, size_t n) {
   unsigned char* p = v;
   while (n--) *p++ = c;
   return v;
}

// 从`src`向`dst`移动`n`个字节。此函数对于内存重叠区域也是安全的。
void* memmove(void* dst, const void* src, size_t n) {
   char* pd = dst;
   const char* ps = src;
   if (ps < pd && pd < ps + n) { // 检查src在dst之前是否有重叠
      pd += n;
      ps += n;
      while (n--) *--pd = *--ps; // 如果有重叠从末尾开始移动
   }
   else {
      while (n--) *pd++ = *ps++; // 无重叠直接复制
   }
   return dst;
}

// 从`in`向`out`复制`n`个字节。此函数不检查内存区域是否重叠。
void* memcpy(void* out, const void* in, size_t n) {
   char* dest = out;
   const char* src = in;
   while (n--) *dest++ = *src++;
   return out;
}

// 比较从`s1`和`s2`开始的`n`个字节。
// 如果相等返回0，如果在比较过程中`s1`小于`s2`返回负值，如果`s1`大于`s2`返回正值。
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
