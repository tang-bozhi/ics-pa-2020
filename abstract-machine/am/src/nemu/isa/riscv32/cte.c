#include <am.h>
#include <riscv32.h>
#include <klib.h>

// 声明一个全局函数指针变量，用于指向用户自定义的事件处理函数
static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context* c) {
  if (user_handler) {
    Event ev = { 0 }; // 初始化事件结构体
    switch (c->cause) {
    case -1:
      ev.event = EVENT_YIELD;
      break;
    case 1:
      ev.event = EVENT_SYSCALL;
      break;
    default:
      ev.event = EVENT_ERROR; // 将所有未识别的中断或异常视为错误
      break;
    }
    c = user_handler(ev, c); // 调用用户定义的事件处理函数
    assert(c != NULL); // 确保返回的上下文不为空  
  }
  return c;
}


// 声明一个外部汇编函数，用于异常处理入口
extern void __am_asm_trap(void);

bool cte_init(Context* (*handler)(Event, Context*)) {
  // 使用汇编指令设置异常处理入口
  asm volatile("csrw stvec, %0" : : "r"(__am_asm_trap));

  // 注册用户自定义的事件处理函数
  user_handler = handler;

  return true; // 返回true表示初始化成功
}

Context* kcontext(Area kstack, void (*entry)(void*), void* arg) {
  return NULL; // 返回NULL表示未实现
}

void yield() {
  asm volatile("li a7, -1; ecall"); // 将-1加载到a7寄存器，并执行系统调用
}

bool ienabled() {
  return false; // 返回false表示中断未使能
}

void iset(bool enable) {
  // 空函数，未实现
}
