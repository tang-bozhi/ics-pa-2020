#include "trap.h"

// #include <stdint.h>
// #include <string.h>
// #include <assert.h>//这三行用于在"make ARCH=riscv32-nemu ALL=memset run"的时候让native使用glibc

#define N 32
uint8_t data[N];

// 初始化 data 数组，赋值为1到N
void reset() {
   int i;
   for (i = 0; i < N; i++) {
      data[i] = i + 1;
   }
}

// 检查[l,r)区间中的值是否依次递增开始于 val
void check_seq(int l, int r, int val) {
   int i;
   for (i = l; i < r; i++) {
      assert(data[i] == val + i - l); // 确保每个位置的值是正确的递增序列
   }
}

// 检查[l,r)区间中的值是否均为val
void check_eq(int l, int r, int val) {
   int i;
   for (i = l; i < r; i++) {
      assert(data[i] == val); // 确保每个位置的值都等于 val
   }
}

// 测试 memset 函数
void test_memset() {
   int l, r;
   for (l = 0; l < N; l++) {
      for (r = l + 1; r <= N; r++) {
         reset(); // 重置 data 数组
         uint8_t val = (l + r) / 2; // 计算填充值
         memset(data + l, val, r - l); // 从 l 开始到 r 填充 val
         check_seq(0, l, 1); // 检查前半部分是否未被修改
         check_eq(l, r, val); // 检查被 memset 修改的部分
         check_seq(r, N, r + 1); // 检查后半部分是否保持原始赋值
      }
   }
}

// 主函数，运行测试
int main() {
   test_memset();
   return 0;
}
