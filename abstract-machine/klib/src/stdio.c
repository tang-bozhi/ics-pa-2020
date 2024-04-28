#include <am.h>          // Abstract Machine (AM) 接口头文件，用于提供硬件抽象层。通常在操作系统或模拟环境中使用，以提供与具体硬件无关的接口。
#include <klib.h>        // KLIB 是一个轻量级的库，它可能包含各种工具和帮助函数，例如内存管理、字符串处理等，具体内容依库的实现而定。
#include <klib-macros.h> // 此头文件可能包含用于KLIB的宏定义，它们可以简化编程，提高代码重用性和可读性。例如，宏可能包括用于调试的宏，或者特定功能的快捷方式。
#include <stdarg.h>      // 标准参数库，用于访问函数的可变参数列表。这在实现如 printf 这样需要处理不定数量参数的函数时非常重要。
#include <limits.h>      // 提供了C标准指定的各种数据类型的极限值。例如，INT_MAX, CHAR_BIT 等，这对于便携性和正确性至关重要。
#include <stdbool.h>     // 提供了布尔类型的支持。在 C99 之前的C语言标准中并未原生支持布尔类型，这个头文件定义了 bool、true、false。
#include <float.h>       // 提供关于浮点数限制和属性的宏。例如，它定义了浮点类型的最大值、最小值、精度等，这对于浮点运算的可靠性和跨平台一致性很重要。


// 只在不使用原生 ISA 或者明确指定使用 KLIB 时编译以下代码
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// 实现 printf 函数
int printf(const char* fmt, ...) {
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
static int print_int(char* buf, int max_len, int value) {
   char temp[12]; // 足够存储 INT_MIN 和空字符
   int pos = 0;
   bool is_negative = value < 0; // 检查是否为负数
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

   int count = 0;
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

// 实现 vsnprintf 函数
int vsnprintf(char* buf, size_t n, const char* fmt, va_list ap) {
   int count = 0;
   while (*fmt && count < n - 1) {
      if (*fmt == '%') {
         fmt++;
         int sublen = 0;
         char temp_buf[64];
         switch (*fmt) {
         case 'd':  // 整数格式化
            sublen = print_int(temp_buf, sizeof(temp_buf), va_arg(ap, int));
            break;
         case 'f':  // 浮点数格式化
            sublen = print_float(temp_buf, sizeof(temp_buf), va_arg(ap, double), 6);
            break;
         case 's':  // 字符串复制
            sublen = print_chars(temp_buf, sizeof(temp_buf), va_arg(ap, char*), INT_MAX);
            break;
         case 'c':  // 字符输出
            temp_buf[0] = (char)va_arg(ap, int);
            sublen = 1;
            break;
         default:  // 处理其他未知格式
            temp_buf[0] = '%';
            temp_buf[1] = *fmt;
            sublen = 2;
            break;
         }
         sublen = print_chars(buf + count, n - count - 1, temp_buf, sublen);
         count += sublen;
         fmt++;
      }
      else {
         buf[count++] = *fmt++; // 复制非格式化部分的字符
      }
   }
   buf[count] = '\0'; // 确保字符串以空字符结尾
   return count;
}

#endif
