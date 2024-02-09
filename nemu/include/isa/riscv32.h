#ifndef __ISA_RISCV32_H__
#define __ISA_RISCV32_H__

#include <common.h>

// memory
#define riscv32_IMAGE_START 0x100000
#define riscv32_PMEM_BASE 0x80000000

// reg

typedef struct {
   struct {
      rtlreg_t _32;
   } gpr[32];

   vaddr_t pc;
} riscv32_CPU_state;

// decode
typedef struct {
   union {
      struct {
         uint32_t opcode1_0 : 2;//opcode1_0 和 opcode6_2：指令操作码的一部分，用于指定指令的类型。
         uint32_t opcode6_2 : 5;
         uint32_t rd : 5;//rd：目标寄存器，用于存储指令执行的结果。
         uint32_t funct3 : 3;//funct3：功能码，与操作码一起确定指令的确切操作。
         uint32_t rs1 : 5;//rs1：第一个源寄存器。
         int32_t  simm11_0 : 12;//simm11_0：12位有符号立即数，用于算术和逻辑操作。
      } i;//I-type（立即数类型）
      struct {
         uint32_t opcode1_0 : 2;
         uint32_t opcode6_2 : 5;
         uint32_t imm4_0 : 5;//imm4_0 和 simm11_5：存储指令中的立即数，用于确定存储位置的偏移量。
         uint32_t funct3 : 3;
         uint32_t rs1 : 5;
         uint32_t rs2 : 5;//其他字段与I-type相似，但rs2代表第二个源寄存器，用于存储到内存的值。
         int32_t  simm11_5 : 7;//imm4_0 和 simm11_5：存储指令中的立即数，用于确定存储位置的偏移量。
      } s;//S-type（存储类型）
      struct {
         uint32_t opcode1_0 : 2;
         uint32_t opcode6_2 : 5;
         uint32_t rd : 5;
         uint32_t imm31_12 : 20;
      } u;//U-type（无符号立即数类型）
      uint32_t val;//这是一个32位无类型字段，可以存储整个指令的值。在解码过程中，可以通过这个字段访问整个指令的原始值，然后根据需要解析为其他格式
   } instr; // 在riscv32_ISADecodeInfo中，instr联合体可以表示RISC - V指令集中的几种不同的指令格式
} riscv32_ISADecodeInfo;

#define isa_vaddr_check(vaddr, type, len) (MEM_RET_OK)
#define riscv32_has_mem_exception() (false)

#endif
