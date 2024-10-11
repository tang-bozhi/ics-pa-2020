#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "NDL.h"  // 包含NDL库

int main() {
   NDL_Init(0);  // 初始化NDL库

   uint32_t start = NDL_GetTicks();  // 获取开始时的时间
   while (1) {
      uint32_t current = NDL_GetTicks();  // 获取当前时间

      // 计算当前时间与初始时间的差值（以毫秒为单位）
      uint32_t elapsed = current - start;

      if (elapsed >= 500) {  // 如果差值大于等于0.5秒（500毫秒）
         printf("Half a second has passed!\n");
         start = current;  // 重置开始时间
      }
   }

   NDL_Quit();  // 结束NDL库
   return 0;
}
