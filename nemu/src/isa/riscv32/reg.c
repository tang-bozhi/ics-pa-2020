#include <isa.h>
#include "local-include/reg.h"

word_t get_reg_value(int i);

const char* regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};//这段代码定义了一个字符串数组regs[]，其中包含的元素代表了寄存器的名称。这种数组通常用于汇编语言编程、模拟器开发，或者任何需要通过寄存器名访问寄存器值的场景。在这个具体的例子中，它显然是用来代表某种处理器架构（很可能是RISC-V，根据寄存器的命名约定来看）中的寄存器名称
//每个元素代表一个特定的寄存器：(参见相关.md)

void isa_reg_display() {
   extern riscv32_CPU_state cpu;  // 假设这是你的CPU状态
   for (int i = 0; i < 32; i++) { // 遍历32个通用寄存器
      printf("%s reg[%d] = 0x%x\n", regs[i], i, cpu.gpr[i]._32); // 打印寄存器名和值
   }
   printf("pc = 0x%x\n", cpu.pc); // 打印程序计数器pc
}


word_t isa_reg_str2val(const char* s, bool* success) {
   if (s == NULL || success == NULL) {
      printf("isa_reg_str2val() failed,Illegal input parameters\n");
      return -1;
   }

   for (int i = 0; i < 32; i++) {
      if (strcmp(s, regs[i]) == 0) {
         *success = true;
         return get_reg_value(i);
      }
   }
   if (strcmp(s, "pc") == 0) {
      *success = true;
      return get_reg_value(32); // 对应于pc的特殊索引
   }

   *success = false;
   return -1;
}

word_t get_reg_value(int i) {
   extern riscv32_CPU_state cpu; // 假设这是你的CPU状态
   if (i >= 0 && i < 32) {
      return cpu.gpr[i]._32; // 返回第i个通用寄存器的值
   }
   else if (i == 32) {
      return cpu.pc; // 如果i为32，返回pc的值
   }
   else {
      printf("Illegal Register Index.\n");
      return -1; // 非法寄存器索引
   }
}

