#include <stdio.h>
#include <sys/time.h>

int main() {
   struct timeval start, current;  // 定义两个timeval结构体，用于记录时间
   gettimeofday(&start, NULL);     // 获取程序开始时的时间
   while (1) {
      gettimeofday(&current, NULL);  // 不断获取当前时间
      // 计算当前时间与初始时间的差值（以微秒为单位）
      long elapsed = (current.tv_sec - start.tv_sec) * 1000000 + (current.tv_usec - start.tv_usec);

      if (elapsed >= 500000) {  // 如果差值大于等于0.5秒（500000微秒）
         printf("Half a second has passed!\n");
         gettimeofday(&start, NULL);  // 重新获取当前时间，作为新的开始时间
      }
   }

   return 0;
}
