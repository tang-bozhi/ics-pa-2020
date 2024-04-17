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
      struct {//R,B,J没有核实
         uint32_t opcode1_0 : 2; // opcode1_0 和 opcode6_2：指令操作码的一部分。
         uint32_t opcode6_2 : 5;
         uint32_t rd : 5;      // rd：目标寄存器。
         uint32_t funct3 : 3;  // funct3：功能码，与操作码一起确定确切的操作。
         uint32_t rs1 : 5;     // rs1：第一个源寄存器。
         uint32_t rs2 : 5;     // rs2：第二个源寄存器。
         uint32_t funct7 : 7;  // funct7：额外的功能码，与funct3一起用于确定确切的操作。
      } r;// R-type指令
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
         uint32_t imm4_0 : 5;  //imm4_0 和 simm11_5：存储指令中的立即数，用于确定存储位置的偏移量。
         uint32_t funct3 : 3;
         uint32_t rs1 : 5;
         uint32_t rs2 : 5;//其他字段与I-type相似，但rs2代表第二个源寄存器，用于存储到内存的值。
         int32_t  simm11_5 : 7;//imm4_0 和 simm11_5：存储指令中的立即数，用于确定存储位置的偏移量。
      } s;//S-type（存储类型）
      struct {
         uint32_t opcode1_0 : 2;
         uint32_t opcode6_2 : 5;
         uint32_t imm11 : 1;   // 分支偏移量的立即数部分。
         uint32_t imm4_1 : 4;
         uint32_t funct3 : 3;
         uint32_t rs1 : 5;
         uint32_t rs2 : 5;
         uint32_t imm10_5 : 6;
         uint32_t imm12 : 1;   // 分支偏移量的立即数部分。
      } b;// B-type指令（用于分支）
      struct {
         uint32_t opcode1_0 : 2;
         uint32_t opcode6_2 : 5;
         uint32_t rd : 5;
         uint32_t imm31_12 : 20;//imm31_12：20位无符号立即数，通常用于长距离跳转和大数值的加载。
      } u;//U-type（无符号立即数类型）
      struct {
         uint32_t opcode1_0 : 2;
         uint32_t opcode6_2 : 5;
         uint32_t rd : 5;
         uint32_t imm19_12 : 8; // 跳转偏移量的立即数部分。
         uint32_t imm11 : 1;
         uint32_t imm10_1 : 10;
         uint32_t imm20 : 1;   // 跳转偏移量的立即数部分。
      } j;// J-type指令（用于跳转）
      uint32_t val;//这是一个32位无类型字段，可以存储整个指令的值。在解码过程中，可以通过这个字段访问整个指令的原始值，然后根据需要解析为其他格式
   } instr; //原始指令值
} riscv32_ISADecodeInfo;//注意instr是union

#define isa_vaddr_check(vaddr, type, len) (MEM_RET_OK)
#define riscv32_has_mem_exception() (false)

#endif
