#include <cpu/exec.h>
#include "local-include/rtl.h"

void raise_intr(DecodeExecState* s, word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  cpu.csr.scause = NO;
  cpu.csr.sepc = epc;
}

void query_intr(DecodeExecState* s) {
}
