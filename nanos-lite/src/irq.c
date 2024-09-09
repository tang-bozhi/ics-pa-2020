#include <common.h>
void do_syscall(Context* c);

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
  case EVENT_YIELD:
    printf("tap EVENT_YIELD\n");
    printf("event ID=%d\nc->GPRx=%d\n", e.event, c->GPRx);
    halt(0);
    break;
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
