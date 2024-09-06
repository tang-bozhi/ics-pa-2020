#include <common.h>
#include "syscall.h"
void do_syscall(Context* c) {
  uintptr_t a[4];
  a[0] = c->GPR1;//type
  a[1] = c->GPR2;//a0
  a[2] = c->GPR3;//a1
  a[3] = c->GPR4;//a2
  switch (a[0]) {
  case SYS_exit:  // 处理 exit 系统调用
    halt(a[1]);
    break;
  case SYS_yield:  // 处理 yield 系统调用
    yield();       // 调用 CTE 的 yield 函数
    a[1] = 0;   // 设置返回值为 0
    break;
  default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
