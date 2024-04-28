#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#include <limits.h>
#include <stdbool.h>
#include <float.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// 使用 vsnprintf 实现 printf
int printf(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  char buffer[1024]; // 定义输出缓冲区
  int printed = vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  for (char *p = buffer; *p; p++)
  { // 将每个字符发送到输出设备
    putch(*p);
  }
  return printed; // 返回打印的字符数
}

// 使用 vsnprintf 实现 vsprintf
int vsprintf(char *out, const char *fmt, va_list ap)
{
  return vsnprintf(out, INT_MAX, fmt, ap); // 使用 INT_MAX 表示缓冲区可以非常大
}

// 使用 vsnprintf 实现 sprintf
int sprintf(char *out, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  int result = vsnprintf(out, INT_MAX, fmt, args); // 使用 INT_MAX 代表潜在无限的缓冲区
  va_end(args);
  return result;
}

// 使用 vsnprintf 实现 snprintf
int snprintf(char *out, size_t n, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  int result = vsnprintf(out, n, fmt, args);
  va_end(args);
  return result;
}

static int print_chars(char *buf, int max_len, const char *str, int len) {
    int i;
    for (i = 0; i < len && i < max_len; i++) {
        buf[i] = str[i];
    }
    return i;
}

static int print_int(char *buf, int max_len, int value) {
    char temp[12]; // enough for INT_MIN and a null terminator
    int pos = 0;
    bool is_negative = value < 0;
    if (is_negative) {
        value = -value;
    }
    do {
        temp[pos++] = (char) ('0' + value % 10);
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

static int print_float(char *buf, int max_len, double value, int precision) {
    if (precision > 10 || precision < 0) {
        precision = 6; // default precision
    }

    char temp[64];
    int count = snprintf(temp, sizeof(temp), "%.*f", precision, value);
    if (count > 0) {
        return print_chars(buf, max_len, temp, count);
    }
    return 0;
}

int vsnprintf(char *buf, size_t n, const char *fmt, va_list ap) {
    int count = 0;
    while (*fmt && count < n - 1) {
        if (*fmt == '%') {
            fmt++;
            int sublen = 0;
            char temp_buf[64];
            switch (*fmt) {
                case 'd':
                    sublen = print_int(temp_buf, sizeof(temp_buf), va_arg(ap, int));
                    break;
                case 'f':
                    sublen = print_float(temp_buf, sizeof(temp_buf), va_arg(ap, double), 6);
                    break;
                case 's':
                    sublen = print_chars(temp_buf, sizeof(temp_buf), va_arg(ap, char *), INT_MAX);
                    break;
                case 'c':
                    temp_buf[0] = (char) va_arg(ap, int);
                    sublen = 1;
                    break;
                default:
                    temp_buf[0] = '%';
                    temp_buf[1] = *fmt;
                    sublen = 2;
                    break;
            }
            sublen = print_chars(buf + count, n - count - 1, temp_buf, sublen);
            count += sublen;
            fmt++;
        } else {
            buf[count++] = *fmt++;
        }
    }
    buf[count] = '\0';
    return count;
}


#endif
