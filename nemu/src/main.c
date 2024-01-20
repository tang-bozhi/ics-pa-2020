#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>


void init_monitor(int, char* []);
void engine_start();
int is_exit_status_bad();

int expr(char* e, bool* success);
char tokens[500][200];  // 根据需要处理的最长表达式调整数组大小

int main(int argc, char* argv[]) {
   init_monitor(argc, argv);
   engine_start();

   // 打开输入文件
   FILE* fp = fopen("/home/ics-pa-2020/nemu/tools/gen-expr/input", "r");
   if (fp == NULL) {
      perror("Error opening file");
      return 1;
   }

   // 用于存储从文件中读取的每一行
   char line[1024];

   // 循环读取文件的每一行
   while (fgets(line, sizeof(line), fp)) {
      int expected;  // 用于存储预期的表达式结果
      char expression[1000];  // 用于存储读取的表达式

      // 将读取的行分解为预期结果和表达
      if (sscanf(line, "%d %[^\n]", &expected, expression) == 2) {
         bool success;  // 表达式计算成功标志

         int result = expr(expression, &success);  // 计算表达式的实际结果

         // 比较预期结果和实际结果
         if (!success || result != expected) {
            printf("Test failed: %s = %u, expected %d\n", expression, result, expected);
            // 可以添加错误处理
            assert(0);
         }
      }
      else {
         printf("Invalid format in input file\n");
      }
   }

   // 关闭文件
   fclose(fp);
   return is_exit_status_bad();
}
