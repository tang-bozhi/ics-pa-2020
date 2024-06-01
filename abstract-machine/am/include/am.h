#ifndef AM_H__
#define AM_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include ARCH_H // this macro is defined in $CFLAGS
// examples: "arch/x86-qemu.h", "arch/native.h", ...

// Memory protection flags
#define MMAP_NONE  0x00000000 // no access
#define MMAP_READ  0x00000001 // can read
#define MMAP_WRITE 0x00000002 // can write

// Memory area for [@start, @end)
typedef struct {
  void* start, * end;
} Area;

// Arch-dependent processor context
typedef struct Context Context;

// An event of type @event, caused by @cause of pointer @ref
typedef struct {
  enum {
    EVENT_NULL = 0,
    EVENT_YIELD, EVENT_SYSCALL, EVENT_PAGEFAULT, EVENT_ERROR,
    EVENT_IRQ_TIMER, EVENT_IRQ_IODEV,
  } event;
  uintptr_t cause, ref;
  const char* msg;
} Event;

// A protected address space with user memory @area
// and arch-dependent @ptr
typedef struct {
  int pgsize;
  Area area;
  void* ptr;
} AddrSpace;

#ifdef __cplusplus
extern "C" {
#endif

  // ----------------------- TRM: Turing Machine -----------------------
  extern   Area        heap;
  void     putch(char ch);
  void     halt(int code) __attribute__((__noreturn__));

  // -------------------- IOE: Input/Output Devices --------------------
  bool     ioe_init(void);
  void     ioe_read(int reg, void* buf);
  void     ioe_write(int reg, void* buf);
#include "amdev.h"

  // ---------- CTE: Interrupt Handling and Context Switching ----------
  bool     cte_init(Context* (*handler)(Event ev, Context* ctx));
  void     yield(void);
  bool     ienabled(void);
  void     iset(bool enable);
  Context* kcontext(Area kstack, void (*entry)(void*), void* arg);

  // ----------------------- VME: Virtual Memory -----------------------
  bool     vme_init(void* (*pgalloc)(int), void (*pgfree)(void*));
  void     protect(AddrSpace* as);
  void     unprotect(AddrSpace* as);
  void     map(AddrSpace* as, void* vaddr, void* paddr, int prot);
  Context* ucontext(AddrSpace* as, Area kstack, void* entry);

  // ---------------------- MPE: Multi-Processing ----------------------
  bool     mpe_init(void (*entry)());
  int      cpu_count(void);
  int      cpu_current(void);
  int      atomic_xchg(int* addr, int newval);

#ifdef __cplusplus
}
#endif

#endif


/*
#ifndef AM_H__
#define AM_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include ARCH_H // 这个宏在 $CFLAGS 中定义
                // 例如: "arch/x86-qemu.h", "arch/native.h", ...

// 内存保护标志
#define MMAP_NONE  0x00000000 // 无访问权限
#define MMAP_READ  0x00000001 // 可读
#define MMAP_WRITE 0x00000002 // 可写

// 表示内存区域 [@start, @end)
typedef struct {
  void *start, *end; // 内存区域的起始和结束地址
} Area;

// 体系结构相关的处理器上下文
typedef struct Context Context;

// 表示一个由 @cause 引发的 @event 类型的事件，并且与指针 @ref 相关
typedef struct {
  enum {
    EVENT_NULL = 0,    // 无事件
    EVENT_YIELD,       // 让出事件
    EVENT_SYSCALL,     // 系统调用事件
    EVENT_PAGEFAULT,   // 页故障事件
    EVENT_ERROR,       // 错误事件
    EVENT_IRQ_TIMER,   // 定时器中断
    EVENT_IRQ_IODEV,   // I/O设备中断
  } event;
  uintptr_t cause;     // 事件的原因
  uintptr_t ref;       // 与事件相关的参考指针
  const char *msg;     // 与事件相关的消息
} Event;

// 一个具有用户内存 @area 和体系结构相关指针 @ptr 的受保护地址空间
typedef struct {
  int pgsize;      // 页大小
  Area area;       // 用户内存区域
  void *ptr;       // 体系结构相关的指针
} AddrSpace;

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------- TRM: 图灵机 -----------------------
extern   Area        heap;              // 堆内存区域
void     putch       (char ch);         // 输出一个字符
void     halt        (int code) __attribute__((__noreturn__)); // 用代码停止系统

// -------------------- IOE: 输入/输出设备 --------------------
bool     ioe_init    (void);            // 初始化I/O设备
void     ioe_read    (int reg, void *buf);   // 从I/O寄存器读取
void     ioe_write   (int reg, void *buf);  // 向I/O寄存器写入
#include "amdev.h"                     // 包含设备相关头文件

// ---------- CTE: 中断处理和上下文切换 ----------
bool     cte_init    (Context *(*handler)(Event ev, Context *ctx)); // 初始化上下文切换和中断处理
void     yield       (void);            // 让出CPU
bool     ienabled    (void);            // 检查中断是否启用
void     iset        (bool enable);     // 设置中断启用标志
Context *kcontext    (Area kstack, void (*entry)(void *), void *arg); // 创建一个内核上下文

// ----------------------- VME: 虚拟内存 -----------------------
bool     vme_init    (void *(*pgalloc)(int), void (*pgfree)(void *)); // 初始化虚拟内存管理
void     protect     (AddrSpace *as);   // 保护一个地址空间
void     unprotect   (AddrSpace *as);   // 取消保护一个地址空间
void     map         (AddrSpace *as, void *vaddr, void *paddr, int prot); // 将虚拟地址映射到物理地址，并设置保护标志
Context *ucontext    (AddrSpace *as, Area kstack, void *entry); // 创建一个用户上下文

// ---------------------- MPE: 多处理 ----------------------
bool     mpe_init    (void (*entry)()); // 初始化多处理
int      cpu_count   (void);            // 获取CPU数量
int      cpu_current (void);            // 获取当前CPU ID
int      atomic_xchg (int *addr, int newval); // 原子交换操作

#ifdef __cplusplus
}
#endif

#endif // AM_H__
*/