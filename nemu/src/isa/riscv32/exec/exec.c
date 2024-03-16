//exec.c：执行相关的C文件，可能包含了程序执行流程的主要逻辑代码
#include <cpu/exec.h>
#include "../local-include/decode.h"
#include "all-instr.h"

static inline void set_width(DecodeExecState* s, int width) {
   if (width != 0) s->width = width;
}

static inline def_EHelper(load) {
   switch (s->isa.instr.i.funct3) {
      EXW(0, ld, 1) // LB
         EXW(1, ld, 2) // LH
         EXW(2, ld, 4) // LW
         EXW(4, ld, 1) // LBU
         EXW(5, ld, 2) // LHU 在RISC-V指令集架构中，LHU（Load Halfword Unsigned）指令是一个I类型（Immediate类型）指令。这类指令主要用于从内存中加载16位无符号半字到寄存器，并将其符号扩展为32位或64位（取决于处理器的位宽）。
   default: exec_inv(s);
   }
}

static inline def_EHelper(store) {
   switch (s->isa.instr.s.funct3) {
      EXW(0, st, 1) //SB（Store Byte）指令用于将一个字节（8位）的数据从寄存器存储到内存中。 
         EXW(1, st, 2) //SH Halfword
         EXW(2, st, 4) //SW Word
   default: exec_inv(s);
   }
}


static inline void fetch_decode_exec(DecodeExecState* s) {
   s->isa.instr.val = instr_fetch(&s->seq_pc, 4);//instructions fetch指令获取:seq_pc（sequential program counter，顺序程序计数器）isa.instr.val（指令集架构中指令的值）
   //实际上，a->b是(*a).b的简写形式
   Assert(s->isa.instr.i.opcode1_0 == 0x3, "Invalid instruction");
   switch (s->isa.instr.i.opcode6_2) {
      IDEX(0b00000, I, load)
         IDEX(0b01000, S, store)
         IDEX(0b01101, U, lui)//lui（Load Upper Immediate）LUI(加载高位立即数)被用于构建32位常量,它使用U类型格式.LUI把32位U立即数值放在目的寄存器rd中,同时把最低的12位用零填充。
         EX(0b11010, nemu_trap)
         IDEX(0b00101, U, auipc)
         IDEX(0b11011, U, jal)
   default: exec_inv(s);
   }
}

static inline void reset_zero() {
   reg_l(0) = 0;
}

vaddr_t isa_exec_once() {
   DecodeExecState s;
   s.is_jmp = 0;
   s.seq_pc = cpu.pc;

   fetch_decode_exec(&s);
   update_pc(&s);

   reset_zero();

   return s.seq_pc;
}
