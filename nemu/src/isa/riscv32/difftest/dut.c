#include <isa.h>
#include <monitor/difftest.h>
#include "../local-include/reg.h"
#include "difftest.h"

bool isa_difftest_checkregs(CPU_state* ref_r, vaddr_t pc) {
  bool match = true;
  for (int i = 0; i < 32; i++) {
    if (reg_l(i) != ref_r->gpr[i]._32) {
      Log("Mismatch in register %s: NEMU = 0x%x, QEMU = 0x%x",
        reg_name(i), reg_l(i), ref_r->gpr[i]._32);
      match = false;
    }
  }
  if (cpu.pc != ref_r->pc) {
    Log("Mismatch in PC: NEMU = 0x%x, QEMU = 0x%x", cpu.pc, ref_r->pc);
    match = false;
  }
  return match;
}


void isa_difftest_attach() {//此函数用于设置与差异测试相关的任何必要状态或配置
  // Currently empty; add any setup required for diff testing here.
}
