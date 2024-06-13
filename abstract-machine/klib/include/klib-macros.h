#ifndef KLIB_MACROS_H__
#define KLIB_MACROS_H__

// 宏：向上取整
#define ROUNDUP(a, sz)      ((((uintptr_t)a) + (sz) - 1) & ~((sz) - 1))
// 宏：向下取整
#define ROUNDDOWN(a, sz)    ((((uintptr_t)a)) & ~((sz) - 1))
// 宏：获取数组长度
#define LENGTH(arr)         (sizeof(arr) / sizeof((arr)[0]))
// 宏：创建区域结构体
#define RANGE(st, ed)       (Area) { .start = (void *)(st), .end = (void *)(ed) }
// 宏：检查指针是否在某个区域内
#define IN_RANGE(ptr, area) ((area).start <= (ptr) && (ptr) < (area).end)

// 宏：将符号转换成字符串
#define STRINGIFY(s)        #s
// 宏：将宏参数转换成字符串
#define TOSTRING(s)         STRINGIFY(s)
// 宏：连接两个宏参数
#define _CONCAT(x, y)       x ## y
#define CONCAT(x, y)        _CONCAT(x, y)

// 宏：输出字符串
#define putstr(s) \
  ({ for (const char *p = s; *p; p++) putch(*p); })

// 宏：从设备读取数据
#define io_read(reg) \
  ({ reg##_T __io_param; \
    ioe_read(reg, &__io_param); \
    __io_param; })

// 宏：向设备写入数据
#define io_write(reg, ...) \
  ({ reg##_T __io_param = (reg##_T) { __VA_ARGS__ }; \
    ioe_write(reg, &__io_param); })

// 宏：编译时断言
#define static_assert(const_cond) \
  static char CONCAT(_static_assert_, __LINE__) [(const_cond) ? 1 : -1] __attribute__((unused))

// 宏：条件性触发 panic
#define panic_on(cond, s) \
  ({ if (cond) { \
      putstr("AM Panic: "); putstr(s); \
      putstr(" @ " __FILE__ ":" TOSTRING(__LINE__) "  \n"); \
      halt(1); \
    } })

// 宏：无条件触发 panic
#define panic(s) panic_on(1, s)

#endif
