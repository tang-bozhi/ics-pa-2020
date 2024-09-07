#include <common.h>
#include "syscall.h"
void do_syscall(Context* c) {
  uintptr_t a[4];
  a[0] = c->gpr[17];//type
  a[1] = c->gpr[10];//a0
  a[2] = c->gpr[11];//a1
  a[3] = c->gpr[12];//a2
  switch (a[0]) {
  case SYS_exit:  // 处理 exit 系统调用
    printf("tap SYS_exit\n");
    halt(a[1]);
    break;
  case SYS_yield:  // 处理 yield 系统调用
    printf("tap SYS_yield\n");
    yield();
    break;
  default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
