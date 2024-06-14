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
   for (i = 0; i < len && i < max_len; i++) {
      buf[i] = str[i];
   }
   return i;
}

// 辅助函数，用于将整数转换为字符串并复制到缓冲区
static int print_int(char* buf, int max_len, int value, int width, char pad) {
   char temp[12]; // 足够存储 INT_MIN 和空字符
   int pos = 0;
   bool is_negative = value < 0;
   if (is_negative) {
      value = -value; // 转换为正数
   }
   do {
      temp[pos++] = (char)('0' + value % 10);
      value /= 10;
   } while (value > 0 && pos < sizeof(temp) - 1);

   if (is_negative) {
      temp[pos++] = '-';
   }

   int needed = width - pos; // 计算需要填充的字符数量
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

// 辅助函数，用于将浮点数转换为字符串并复制到缓冲区
static int print_float(char* buf, int max_len, double value, int precision) {
   if (precision > 10 || precision < 0) {
      precision = 6; // 默认精度
   }

   char temp[64];
   int count = snprintf(temp, sizeof(temp), "%.*f", precision, value); // 使用 snprintf 格式化浮点数
   if (count > 0) {
      return print_chars(buf, max_len, temp, count);
   }
   return 0;
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
static int print_long(char* buf, int max_len, long value, int width, char pad) {
   char temp[22];  // 缓冲区大小足以容纳长整型
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

         // 解析精度
         if (*fmt == '.') {
            fmt++;  // 跳过 '.'
            precision = 0;  // 初始化精度
            while (*fmt >= '0' && *fmt <= '9') {
               precision = precision * 10 + (*fmt - '0');
               fmt++;
            }
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
         fmt++;  // 移动到下一个字符

         // 将格式化后的临时缓冲区内容复制到主缓冲区
         sublen = print_chars(buf + count, n - count - 1, temp_buf, sublen);
         count += sublen;  // 更新已写入字符数
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
