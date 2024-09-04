#include <common.h>
void do_syscall(Context* c);

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
  case EVENT_YIELD:
    printf("Yield event recognized!\n");
    break;
  case EVENT_SYSCALL:
    printf("Syscall event recognized!\n");
    do_syscall(Context * c);
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
