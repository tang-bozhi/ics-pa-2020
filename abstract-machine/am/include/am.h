#ifndef AM_H__
#define AM_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include ARCH_H // this macro is defined in $CFLAGS
// examples: "arch/x86-qemu.h", "arch/native.h", ...
// 这个宏在编译时通过 $CFLAGS 定义，用于包含特定架构的头文件，例如 "arch/x86-qemu.h", "arch/native.h"

// Memory protection flags 内存保护标志
#define MMAP_NONE  0x00000000 // no access 无访问权限
#define MMAP_READ  0x00000001 // can read 可读
#define MMAP_WRITE 0x00000002 // can write 可写

// Memory area for [@start, @end) 表示内存区域的结构体，用于定义一个内存区间 [@start, @end)
typedef struct {
  void* start, * end;// 起始地址 // 结束地址（不包含）
} Area;

// Arch-dependent processor context 与架构相关的处理器上下文结构体
typedef struct Context Context;//将struct Context定义为Context

// An event of type @event, caused by @cause of pointer @ref 事件类型定义
typedef struct {
  enum {
    EVENT_NULL = 0,
    EVENT_YIELD,// 让出CPU时间
    EVENT_SYSCALL,// 系统调用
    EVENT_PAGEFAULT,// 页面错误
    EVENT_ERROR,// 错误
    EVENT_IRQ_TIMER,// 定时器中断请求
    EVENT_IRQ_IODEV,// IO设备中断请求
  } event;
  uintptr_t cause, ref;// 事件原因 // 事件相关引用
  const char* msg;// 事件相关消息
} Event;

// A protected address space with user memory @area
// and arch-dependent @ptr  // 带保护的地址空间结构，包含用户内存区域和与架构相关的指针
typedef struct {
  int pgsize;// 页面大小
  Area area;// 内存区域
  void* ptr;// 与架构相关的指针
} AddrSpace;

#ifdef __cplusplus
extern "C" {
#endif

  // ----------------------- TRM: Turing Machine -----------------------
  // ----------------------- TRM: 图灵机 -----------------------
  extern   Area        heap;
  void     putch(char ch);
  void     halt(int code) __attribute__((__noreturn__));

  // -------------------- IOE: Input/Output Devices --------------------
  // -------------------- IOE: 输入/输出设备 --------------------
  bool     ioe_init(void);
  void     ioe_read(int reg, void* buf);
  void     ioe_write(int reg, void* buf);
#include "amdev.h"// 包含设备定义文件

  // ---------- CTE: Interrupt Handling and Context Switching ----------
  // ---------- CTE: 中断处理和上下文切换 ----------
  bool cte_init(Context* (*handler)(Event ev, Context* ctx));  // 初始化中断处理
  void yield(void);  // 让出CPU时间
  bool ienabled(void);  // 检查中断是否启用
  void iset(bool enable);  // 设置中断启用状态
  Context* kcontext(Area kstack, void (*entry)(void*), void* arg);  // 创建内核上下文

  // ----------------------- VME: Virtual Memory -----------------------
  // ----------------------- VME: 虚拟内存 -----------------------
  bool vme_init(void* (*pgalloc)(int), void (*pgfree)(void*));  // 初始化虚拟内存
  void protect(AddrSpace* as);  // 保护地址空间
  void unprotect(AddrSpace* as);  // 取消保护地址空间
  void map(AddrSpace* as, void* vaddr, void* paddr, int prot);  // 映射虚拟地址到物理地址
  Context* ucontext(AddrSpace* as, Area kstack, void* entry);  // 创建用户上下文

  // ---------------------- MPE: Multi-Processing ----------------------
  // ---------------------- MPE: 多处理器处理 ----------------------
  bool mpe_init(void (*entry)());  // 初始化多处理器环境
  int cpu_count(void);  // 获取CPU数量
  int cpu_current(void);  // 获取当前CPU索引
  int atomic_xchg(int* addr, int newval);  // 原子交换操作

#ifdef __cplusplus
}
#endif

#endif

