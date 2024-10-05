#include <common.h>
void do_syscall(Context* c);

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
  case EVENT_YIELD://__am_irq_handle没有设置这种情况,进入了syscall
    //printf("tap EVENT_YIELD\n");
    //printf("event ID=%d\nc->GPRx=%d\n", e.event, c->GPRx);
    halt(0);
    break;
  case EVENT_SYSCALL:
    //printf("Tap EVENT_SYSCALL\n");
    do_syscall(c);
    //printf("Exit EVENT_SYSCALL\n");
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
