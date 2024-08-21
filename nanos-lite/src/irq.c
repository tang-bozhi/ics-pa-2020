#include <common.h>

static Context* do_event(Event e, Context* c) {
  printf("tap do_event\n");
  switch (e.event) {
  case EVENT_YIELD:
    printf("Yield event recognized!\n");
    break;
  case EVENT_SYSCALL:
    printf("Syscall event recognized!\n");
    break;
  case EVENT_PAGEFAULT:
    printf("Pagefault event recognized!\n");
    break;
  case EVENT_ERROR:
    printf("Error event recognized!\n");
    break;
  case EVENT_IRQ_TIMER:
    printf("Timer IRQ event recognized!\n");
    break;
  case EVENT_IRQ_IODEV:
    printf("IO device IRQ event recognized!\n");
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
