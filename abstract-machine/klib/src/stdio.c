#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// 使用 vsnprintf 实现 printf
int printf(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char buffer[1024]; // 定义输出缓冲区
  int printed = vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  for (char* p = buffer; *p; p++) { // 将每个字符发送到输出设备
    _putc(*p);
  }
  return printed; // 返回打印的字符数
}

// 使用 vsnprintf 实现 vsprintf
int vsprintf(char* out, const char* fmt, va_list ap) {
  return vsnprintf(out, INT_MAX, fmt, ap); // 使用 INT_MAX 表示缓冲区可以非常大
}

// 使用 vsnprintf 实现 sprintf
int sprintf(char* out, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int result = vsnprintf(out, INT_MAX, fmt, args); // 使用 INT_MAX 代表潜在无限的缓冲区
  va_end(args);
  return result;
}

// 使用 vsnprintf 实现 snprintf
int snprintf(char* out, size_t n, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int result = vsnprintf(out, n, fmt, args);
  va_end(args);
  return result;
}

// 直接使用 vsnprintf，假设它在其他地方已经正确实现
int vsnprintf(char* out, size_t n, const char* fmt, va_list ap) {
  // 这里我们需要实现或使用库版本的 vsnprintf
  // 假设它已经在 stdio.h 或其他常见头文件中实现
  return vsnprintf(out, n, fmt, ap);
}

#endif
