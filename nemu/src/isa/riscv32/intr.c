#include <cpu/exec.h>
#include "local-include/rtl.h"

void raise_intr(DecodeExecState* s, word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  if (NO == -1) {//yield情况
    epc += 4;
  }
  cpu.csr.scause = NO;
  cpu.csr.sepc = epc;//当前pc
  // 从 stvec 寄存器中取出异常入口地址
  vaddr_t stvec = cpu.csr.stvec;

  // 设置跳转标志和跳转地址
  s->is_jmp = 1;
  s->jmp_pc = stvec;
}

void query_intr(DecodeExecState* s) {
}
