#include "trap.h"

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>


void test_malloc_free_basic() {
   printf("Testing basic malloc and free...\n");
   int* ptr = malloc(sizeof(int));
   if (ptr == NULL) {
      printf("malloc failed to allocate memory\n");
   }
   else {
      *ptr = 12345;  // 写入数据
      printf("Data written: %d\n", *ptr);
      free(ptr);  // 释放内存
      printf("Memory freed successfully\n");
   }
}

void test_malloc_zero() {
   printf("Testing malloc with zero size...\n");
   void* ptr = malloc(0);
   if (ptr == NULL) {
      printf("malloc(0) returned NULL as expected\n");
   }
   else {
      free(ptr);
      printf("malloc(0) did not return NULL, but memory was freed\n");
   }
}

void test_free_null() {
   printf("Testing free with NULL pointer...\n");
   free(NULL);  // 应该什么都不做
   printf("free(NULL) executed without errors\n");
}

void test_malloc_free_random_sizes() {
   printf("Testing random size memory allocations...\n");
   srand(79589);  // 设置随机种子
   for (int i = 0; i < 100; i++) {
      size_t size = rand() % 1500;  // 随机大小，最大1023字节
      char* buffer = malloc(size);
      if (buffer == NULL) {
         printf("Failed to allocate %zu bytes\n", size);
         continue;
      }
      memset(buffer, 0xAB, size);  // 用0xAB填充
      free(buffer);
   }
   printf("Random sizes memory allocations tested successfully\n");
}

int main() {
   test_malloc_free_basic();
   test_malloc_zero();
   test_free_null();
   test_malloc_free_random_sizes();

   return 0;
}
