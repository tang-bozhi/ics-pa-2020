#include <common.h>
void do_syscall(Context* c);

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    // case EVENT_YIELD:
    //   printf("tap EVENT_YIELD\n");
    //   halt(0);
    //   break;//参考个人的答案当中有这个case1,但是前面的__am_irq_handle没有这个事件的分发了,yield被归为do_syscall的小项情况
  case EVENT_SYSCALL:
    printf("tap EVENT_SYSCALL\n");
    do_syscall(c);
    break;
  default:
    panic("Unhandled event ID = %d", e.event);
    break;
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
