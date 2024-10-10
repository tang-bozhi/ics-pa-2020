#include <common.h>
#include "syscall.h"
#include <fs.h>
#include <sys/time.h>
#include <am.h>

// void write(intptr_t* buf, size_t count) {
//   for (int i = 0; i < count; i++) {
//     putch(*((char*)buf + i));
//   }nanos-lite/src/syscall.c
// }

void sbrk(intptr_t increment) {

}

void sys_gettimeofday(struct timeval* tv, struct timezone* tz) {
  uint64_t us = io_read(AM_TIMER_UPTIME).us;
  tv->tv_sec = us / 1000000;
  tv->tv_usec = us - us / 1000000 * 1000000;
}

void do_syscall(Context* c) {
  uintptr_t a[4];
  a[0] = c->gpr[17];//type a7
  a[1] = c->gpr[10];//a0
  a[2] = c->gpr[11];//a1
  a[3] = c->gpr[12];//a2
  switch (a[0]) {
  case SYS_exit:  // 处理 exit 系统调用
    //printf("tap SYS_exit\n");
    halt(0);
    break;
  case SYS_yield:
    //printf("tap SYS_yield\n");
    yield();
    break;
  case SYS_open:
    //printf("tap SYS_open\n");
    c->gpr[10] = fs_open((const char*)a[1], a[2], a[3]);
    //printf("c->gpr[17] = %d\n", c->gpr[17]);
    //printf("c->gpr[10] = %d\n", c->gpr[10]);
    //printf("c->gpr[11] = %d\n", c->gpr[11]);
    //printf("c->gpr[12] = %d\n", c->gpr[12]);
    break;
  case SYS_read:
    //printf("tap SYS_read\n");
    c->gpr[10] = fs_read(a[1], (void*)a[2], a[3]);
    break;
  case SYS_write:
    //printf("tap SYS_write\n");
    c->gpr[10] = fs_write(a[1], (const void*)a[2], a[3]);
    break;
  case SYS_lseek:
    //printf("tap SYS_lseek\n");
    c->gpr[10] = fs_lseek(a[1], a[2], a[3]);
    break;
  case SYS_close:
    //printf("tap SYS_close\n");
    c->gpr[10] = fs_close(a[1]);
    break;
  case SYS_brk:
    //printf("tap SYS_brk\n");
    //c->gpr[10] = sbrk(a[1]);
    sbrk(a[1]);
    c->gpr[10] = 0;
    break;
  case SYS_gettimeofday:
    sys_gettimeofday((struct timeval*)a[1], (struct timezone*)a[2]);//a[1]:a0,a[2]:a1
    break;
  default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
