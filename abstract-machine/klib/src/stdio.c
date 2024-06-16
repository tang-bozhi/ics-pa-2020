#include <am.h> 
#include <klib.h>  
#include <klib-macros.h>
#include <stdarg.h>
#include <limits.h>
#include <stdbool.h>
#include <float.h>


// 只在不使用原生 ISA 或者明确指定使用 KLIB 时编译以下代码
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// 实现 printf 函数
int printf(const char* fmt, ...) {// fmt 格式化字符串
   va_list args;                  // 定义一个 va_list 类型的变量用于访问可变参数
   va_start(args, fmt);           // 初始化 args 变量，让它指向第一个可变参数
   char buffer[1024];             // 定义一个字符数组作为输出缓冲区
   int printed = vsnprintf(buffer, sizeof(buffer), fmt, args); // 使用 vsnprintf 函数进行格式化
   va_end(args);                  // 结束可变参数的获取
   for (char* p = buffer; *p; p++) {  // 遍历格式化后的字符串
      putch(*p);                   // 使用 putch 函数输出每一个字符
   }
   return printed;                // 返回打印的字符数
}

// 实现 vsprintf 函数
int vsprintf(char* out, const char* fmt, va_list ap) {
   return vsnprintf(out, INT_MAX, fmt, ap); // 调用 vsnprintf 函数进行格式化，假设缓冲区足够大
}

// 实现 sprintf 函数
int sprintf(char* out, const char* fmt, ...) {
   va_list args;
   va_start(args, fmt);
   int result = vsnprintf(out, INT_MAX, fmt, args); // 调用 vsnprintf 函数进行格式化，假设缓冲区足够大
   va_end(args);
   return result;
}

// 实现 snprintf 函数
int snprintf(char* out, size_t n, const char* fmt, ...) {
   va_list args;
   va_start(args, fmt);
   int result = vsnprintf(out, n, fmt, args);  // 调用 vsnprintf 进行格式化，n 指定最大字符数
   va_end(args);
   return result;
}

// 辅助函数，用于复制字符串到缓冲区
static int print_chars(char* buf, int max_len, const char* str, int len) {
   int i;
   // 循环条件中加入对字符串结束符的检查
   for (i = 0; i < len && i < max_len && str[i] != '\0'; i++) {
      buf[i] = str[i];
   }
   // 确保输出字符串正确终止
   if (i < max_len) {
      buf[i] = '\0'; // 在不超过最大长度的情况下添加字符串终止符
   }
   return i; // 返回复制的字符数量，不包含终止符
}


// 辅助函数，用于将整数转换为字符串并复制到缓冲区
static int print_int(char* buf, int max_len, int value, int width, char pad) {
   char temp[12]; // 足够存储 INT_MIN 和空字符
   int pos = 0; // 临时数组的索引，用于存储生成的字符
   bool is_negative = value < 0; // 判断整数是否为负数
   if (is_negative) {
      value = -value; // 如果为负，将整数转换为正数
   }
   do {
      temp[pos++] = (char)('0' + value % 10); // 取整数的最后一位数字，转换为字符后存储
      value /= 10; // 移除已处理的最后一位数字
   } while (value > 0 && pos < sizeof(temp) - 1); // 继续处理直至整数部分为0或缓冲区满

   if (is_negative) {
      temp[pos++] = '-'; // 如果原整数为负，添加负号
   }

   int needed = width - pos; // 需要填充的字符数量，由指定的宽度和已用字符数决定
   int count = 0; // 用于记录已经复制到主缓冲区的字符数
   while (needed > 0 && count < max_len) {
      buf[count++] = pad; // 在数字前填充字符，直至达到指定宽度
      needed--;
   }

   for (int i = pos - 1; i >= 0 && count < max_len; i--) {
      buf[count++] = temp[i]; // 将数字字符从临时缓冲区反向复制到主缓冲区，形成正确的顺序
   }
   return count; // 返回已复制到主缓冲区的字符总数
}

// 辅助函数，用于将浮点数转换为字符串并复制到缓冲区
static int print_float(char* buf, int max_len, double value, int precision) {
   if (precision < 0 || precision > 10) precision = 6;  // 限制精度范围

   int count = 0;
   if (value < 0) {
      if (count < max_len) {
         buf[count++] = '-';
      }
      value = -value;  // 处理负数
   }

   // 处理整数部分
   long long int_part = (long long)value;  // 获取整数部分
   char int_buf[20];  // 临时缓冲区存储整数部分
   int int_pos = 0;
   do {
      int_buf[int_pos++] = '0' + int_part % 10;  // 取最低位
      int_part /= 10;
   } while (int_part > 0 && int_pos < sizeof(int_buf));

   // 整数部分从高位到低位复制到输出缓冲区
   for (int i = int_pos - 1; i >= 0 && count < max_len; i--) {
      buf[count++] = int_buf[i];
   }

   // 处理小数部分
   if (precision > 0 && count < max_len) {
      buf[count++] = '.';  // 添加小数点
   }

   double frac_part = value - (long long)value;  // 获取小数部分
   for (int i = 0; i < precision && count < max_len; i++) {
      frac_part *= 10;
      int digit = (int)frac_part;
      buf[count++] = '0' + digit;
      frac_part -= digit;
   }

   return count;
}

//添加了一个 print_size_t 辅助函数来处理 size_t 类型的数据
static int print_size_t(char* buf, int max_len, size_t value) {
   char temp[20];
   int pos = 0;
   do {
      temp[pos++] = (char)('0' + value % 10);
      value /= 10;
   } while (value > 0 && pos < sizeof(temp) - 1);

   int count = 0;
   for (int i = pos - 1; i >= 0 && count < max_len; i--) {
      buf[count++] = temp[i];
   }
   return count;
}

// 辅助函数，用于将 unsigned long 转换为字符串并复制到缓冲区
static int print_unsigned_long(char* buf, int max_len, unsigned long value) {
   char temp[21]; // 足够大以存储最大的 unsigned long 值和空字符
   int pos = 0;
   do {
      temp[pos++] = (char)('0' + value % 10);
      value /= 10;
   } while (value > 0 && pos < sizeof(temp) - 1);

   int count = 0;
   for (int i = pos - 1; i >= 0 && count < max_len; i--) {
      buf[count++] = temp[i];
   }
   return count;
}

// 为 long 和 long long 添加辅助函数
// 辅助函数，用于将 long 类型整数转换为字符串并复制到缓冲区
static int print_long(char* buf, int max_len, long value, int width, char pad) {
   char temp[22];  // 缓冲区大小足以容纳长整型数字
   int pos = 0;  // 临时存储转换结果的位置指针
   bool is_negative = value < 0;  // 检查数值是否为负

   // 使用 unsigned long 安全处理负数的绝对值
   unsigned long abs_value = (is_negative) ? -(unsigned long)value : (unsigned long)value;

   // 从个位开始将整数部分转换为字符，存储在临时数组中
   do {
      temp[pos++] = '0' + (abs_value % 10);  // 将当前最低位转换为字符
      abs_value /= 10;  // 移除当前最低位
   } while (abs_value > 0 && pos < sizeof(temp) - 1);

   // 如果原数是负数，添加负号
   if (is_negative) {
      temp[pos++] = '-';
   }

   int needed = width - pos;  // 计算需要的填充字符数，即指定宽度减去数字字符数量
   int count = 0;  // 计数器，记录已复制到输出缓冲区的字符数

   // 在数字之前向输出缓冲区添加填充字符
   while (needed > 0 && count < max_len && count + pos < width) {
      buf[count++] = pad;  // 将填充字符复制到输出缓冲区
      needed--;  // 减少所需的填充字符数量
   }

   // 将数字字符从临时数组反序复制到输出缓冲区
   for (int i = pos - 1; i >= 0 && count < max_len; i--) {
      buf[count++] = temp[i];
   }

   return count;  // 返回已复制到输出缓冲区的字符数
}

static int print_long_long(char* buf, int max_len, long long value, int width, char pad) {
   char temp[22];  // 缓冲区大小足以容纳长长整型
   int pos = 0;
   bool is_negative = value < 0;
   if (is_negative) {
      value = -value;
   }
   do {
      temp[pos++] = (char)('0' + value % 10);
      value /= 10;
   } while (value > 0 && pos < sizeof(temp) - 1);

   if (is_negative) {
      temp[pos++] = '-';
   }

   int needed = width - pos;
   int count = 0;
   while (needed > 0 && count < max_len) {
      buf[count++] = pad;
      needed--;
   }

   for (int i = pos - 1; i >= 0 && count < max_len; i--) {
      buf[count++] = temp[i];
   }

   return count;
}

// 实现 vsnprintf 函数，这是一个可变参数的字符串格式化函数。
int vsnprintf(char* buf, size_t n, const char* fmt, va_list ap) {
   int count = 0;  // 用于统计已写入的字符数

   // 遍历格式字符串，直到遇到字符串结束或达到缓冲区限制
   while (*fmt && count < n - 1) {
      // 检查是否是格式指示符
      if (*fmt == '%') {
         fmt++;  // 跳过 '%'
         int width = 0, precision = -1;  // 宽度和精度初始化
         char pad = ' ';  // 填充字符默认为空格

         // 解析填充字符和宽度 
         if (*fmt == '0') {
            pad = '0';
            fmt++;
         }
         while (*fmt >= '0' && *fmt <= '9') {
            width = width * 10 + (*fmt - '0');
            fmt++;
         }

         char temp_buf[128];  // 临时缓冲区，用于存储单个转换结果
         int sublen = 0;  // 单个转换结果的长度

         // 根据格式说明符进行处理
         switch (*fmt) {
         case 'd':  // 整数
         case 'i':  // 整数
            sublen = print_int(temp_buf, sizeof(temp_buf), va_arg(ap, int), width, pad);
            break;
         case 'f':  // 浮点数
            sublen = print_float(temp_buf, sizeof(temp_buf), va_arg(ap, double), precision);
            break;
         case 's':  // 字符串
            sublen = print_chars(temp_buf, sizeof(temp_buf), va_arg(ap, char*), INT_MAX);
            break;
         case 'c':  // 字符
            temp_buf[0] = (char)va_arg(ap, int);
            sublen = 1;
            break;
         case 'l':  // 长整型或长长整型
            fmt++;
            if (*fmt == 'l') {  // 处理长长整型
               fmt++;
               if (*fmt == 'd' || *fmt == 'i') {
                  sublen = print_long_long(temp_buf, sizeof(temp_buf), va_arg(ap, long long), width, pad);
               }
            }
            else if (*fmt == 'd' || *fmt == 'i') {  // 长整型
               sublen = print_long(temp_buf, sizeof(temp_buf), va_arg(ap, long), width, pad);
            }
            else if (*fmt == 'u') {  // 无符号长整型
               sublen = print_unsigned_long(temp_buf, sizeof(temp_buf), va_arg(ap, unsigned long));
               fmt++;
            }
            break;
         case 'z':  // 处理 size_t
            if (*(fmt + 1) == 'u') {
               fmt++;
               sublen = print_size_t(temp_buf, sizeof(temp_buf), va_arg(ap, size_t));
            }
            fmt++;
            break;
         default:  // 处理未识别的格式
            temp_buf[0] = '%';
            temp_buf[1] = *fmt;
            sublen = 2;
            break;
         }
         // 将格式化后的临时缓冲区内容复制到主缓冲区
         sublen = print_chars(buf + count, n - count - 1, temp_buf, sublen);
         count += sublen;  // 更新已写入字符数
         fmt++;  // 移动到下一个字符
      }
      else {
         // 直接复制非格式化部分的字符
         buf[count++] = *fmt++;
      }
   }
   buf[count] = '\0';  // 确保字符串以空字符结尾
   return count;
}

#endif
