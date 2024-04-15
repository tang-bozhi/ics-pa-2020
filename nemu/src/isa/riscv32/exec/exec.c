// exec.c：执行相关的C文件，可能包含了程序执行流程的主要逻辑代码
#include "../local-include/decode.h"
#include "all-instr.h"
#include <cpu/exec.h>

static inline void set_width(DecodeExecState* s, int width) {
   if (width != 0)
      s->width = width;
}

static inline def_EHelper(load) {
   switch (s->isa.instr.i.funct3) {
      EXW(0, ld, 1); // LB
      EXW(1, ld, 2); // LH
      EXW(2, ld, 4); // LW
      EXW(4, ld, 1); // LBU
      EXW(5, ld, 2); // LHU 在RISC-V指令集架构中，LHU（Load Halfword Unsigned）指令是一个I类型（Immediate类型）指令。这类指令主要用于从内存中加载16位无符号半字到寄存器，并将其符号扩展为32位或64位（取决于处理器的位宽）。
   default:
      exec_inv(s);
   }
}

static inline def_EHelper(store) {
   switch (s->isa.instr.s.funct3) {
      EXW(0, st, 1); // SB（Stor
      // Byte）指令用于将一个字节（8位）的数据从寄存器存储到内存中。
      EXW(1, st, 2); // SH Halfword
      EXW(2, st, 4); // SW Word
   default:
      exec_inv(s);
   }
}

static inline def_EHelper(branch) {
   switch (s->isa.instr.b.funct3) {
      EX(0, beq);
      EX(1, bne);
      EX(4, blt);
      EX(5, bge);
      EX(6, bltu);
      EX(7, bgeu);
   default:
      exec_inv(s);
   }
}

static inline def_EHelper(imm) {
   switch (s->isa.instr.i.funct3) {
      EX(0, addi);
      EX(2, slti);
      EX(3, sltiu);
      EX(4, xori);
      EX(6, ori);
      EX(7, andi);
      EX(1, slli);
   case 5:
      switch (s->isa.instr.r.funct7) { // 使用了union的性质,懒得作位的操作了
         EX(0b0000000, srli);
         EX(0b0100000, srai);
      }
   default:
      exec_inv(s);
   }
}

static inline def_EHelper(reg) {
   switch (s->isa.instr.r.funct3) {
   case 0:
      switch (s->isa.instr.r.funct7) {
         EX(0, add);
         EX(1, sub);
      }
      EX(1, sll);
      EX(2, slt);
      EX(3, sltu);
      EX(4, xor);
   case 5:
      switch (s->isa.instr.r.funct7) {
         EX(0, srl);
         EX(1, sra);
      }
      EX(6, or );
      EX(7, and);
   default:
      exec_inv(s);
   }
}

static inline void fetch_decode_exec(DecodeExecState* s) {
   s->isa.instr.val = instr_fetch(&s->seq_pc, 4); // instructions fetch指令获取:seq_pc（sequential program
   // counter，顺序程序计数器）isa.instr.val（指令集架构中指令的值）
   // 实际上，a->b是(*a).b的简写形式
   Assert(s->isa.instr.i.opcode1_0 == 0x3, "Invalid instruction");
   switch (s->isa.instr.i.opcode6_2) {
      IDEX(0b00000, I, load);
      IDEX(0b01000, S, store);
      IDEX(0b01101, U, lui); // lui（Load Upper Immediate）LUI(加载高位立即数)被用于构建32位常量,它使用U类型格式.LUI把32位U立即数值放在目的寄存器rd中,同时把最低的12位用零填充。
      EX(0b11010, nemu_trap);
      IDEX(0b00101, U, auipc);
      IDEX(0b11011, J, jal);
      IDEX(0b11001, I, jalr);
      IDEX(0b11000, B, branch);
      IDEX(0b00100, I, imm);
      IDEX(0b01100, R, reg);
   default:
      exec_inv(s);
   }
}

static inline void reset_zero() { reg_l(0) = 0; }

vaddr_t isa_exec_once() {
   DecodeExecState s;
   s.is_jmp = 0;
   s.seq_pc = cpu.pc;

   fetch_decode_exec(&s);
   s.seq_pc += 4;//+=长度://instr_fetch函数:(*pc) += len;//危险修改:修改了框架代码,将这一步骤移动到了指令执行完之后

   update_pc(&s);

   reset_zero();

   return s.seq_pc;
}
