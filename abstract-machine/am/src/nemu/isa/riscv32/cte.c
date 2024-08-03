#include <am.h>
#include <riscv32.h>
#include <klib.h>

// 声明一个全局函数指针变量，用于指向用户自定义的事件处理函数
static Context* (*user_handler)(Event, Context*) = NULL;

/*
 * 中断处理函数
 * 这个函数在中断或异常发生时被调用。
 * 它根据上下文中的cause字段生成一个事件，并调用用户自定义的事件处理函数。
 * 最终返回新的上下文。
 */
Context* __am_irq_handle(Context* c) {
  if (user_handler) {
    Event ev = { 0 }; // 初始化事件结构体
    switch (c->cause) {
    case 11: // 11 是 RISC-V 中 ecall 异常的 cause 值，表示自陷异常
      ev.event = EVENT_YIELD;
      break;
    case 0x80000001: // 定时器中断的 cause 值（高位表示中断，低位表示定时器中断）
      ev.event = EVENT_IRQ_TIMER;
      break;
    case 0x80000009: // IO设备中断的 cause 值（高位表示中断，低位表示IO设备中断）
      ev.event = EVENT_IRQ_IODEV;
      break;
    case 12: // 系统调用异常的 cause 值（此值根据具体实现确定）
      ev.event = EVENT_SYSCALL;
      break;
    case 13: // 页面错误的 cause 值（此值根据具体实现确定）
      ev.event = EVENT_PAGEFAULT;
      break;
    default:
      ev.event = EVENT_ERROR; // 将所有未识别的中断或异常视为错误
      break;
    }
    // ev.cause = c->cause;
    // ev.ref = 0; // 根据需要设置 ref 字段
    // ev.msg = "Interrupt or exception"; // 根据需要设置 msg 字段
    ////上方三行应该放入具体的事件处理而不是事件判断之后
    c = user_handler(ev, c); // 调用用户定义的事件处理函数
    assert(c != NULL); // 确保返回的上下文不为空  
  }
  return c;
}


// 声明一个外部汇编函数，用于异常处理入口
extern void __am_asm_trap(void);

/*
 * 中断和异常初始化函数
 * 这个函数用于设置异常处理入口并注册用户自定义的事件处理函数。
 */
bool cte_init(Context* (*handler)(Event, Context*)) {
  // 使用汇编指令设置异常处理入口
  asm volatile("csrw stvec, %0" : : "r"(__am_asm_trap));

  // 注册用户自定义的事件处理函数
  user_handler = handler;

  return true; // 返回true表示初始化成功
}

/*
 * 创建上下文函数
 * 这个函数用于创建一个新的上下文，目前尚未实现。
 */
Context* kcontext(Area kstack, void (*entry)(void*), void* arg) {
  return NULL; // 返回NULL表示未实现
}

/*
 * 让出CPU函数
 * 这个函数通过系统调用触发进程让出CPU的操作。
 */
void yield() {
  asm volatile("li a7, -1; ecall"); // 将-1加载到a7寄存器，并执行系统调用
}

/*
 * 中断使能查询函数
 * 这个函数用于查询中断是否使能，目前总是返回false。
 */
bool ienabled() {
  return false; // 返回false表示中断未使能
}

/*
 * 中断使能设置函数
 * 这个函数用于设置中断使能或禁止，目前未实现。
 */
void iset(bool enable) {
  // 空函数，未实现
}
