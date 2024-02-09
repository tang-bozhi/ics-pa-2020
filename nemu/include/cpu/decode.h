#ifndef __CPU_DECODE_H__
#define __CPU_DECODE_H__

#include <isa.h>

#define OP_STR_SIZE 40//OP_STR_SIZE 定义为40，可能用于限制操作数字符串的最大长度。
enum { OP_TYPE_REG, OP_TYPE_MEM, OP_TYPE_IMM };

typedef struct {
   uint32_t type;//type字段表明操作数的类型，如寄存器、内存地址或立即数。
   int width;//width字段指示操作数的数据宽度，即操作数的大小（例如，8位、16位、32位等）。
   union {
      uint32_t reg;//reg：如果操作数是寄存器类型的，这里存储寄存器的编号。
      word_t imm;//imm和simm：用于存储立即数值，
      sword_t simm;//其中simm是有符号立即数，用于需要表示负数的情况。
   };
   rtlreg_t* preg;//preg是一个指向寄存器的指针，可能用于存储操作结果。
   rtlreg_t val;//val字段存储操作数的实际值，例如，在指令执行过程中计算得到的值。
   char str[OP_STR_SIZE];//str是一个字符数组，可能用于调试目的，存储操作数的文本表示。
} Operand;
/*Operand 结构体用于描述一个操作数的信息，包括类型（寄存器、内存
或立即数）、宽度、具体值（寄存器编号、立即数、有符号立即数）、指向
结果寄存器的指针、实际值，以及一个字符串用于调试或显示*/

typedef struct {
   uint32_t opcode;//opcode：存储当前指令的操作码，即指令的唯一标识，用于确定CPU需要执行的操作类型。
   vaddr_t seq_pc;  // sequential pc//opcode：存储当前指令的操作码，即指令的唯一标识，用于确定CPU需要执行的操作类型。
   uint32_t is_jmp;
   vaddr_t jmp_pc;//如果is_jmp为真，这个字段存储跳转指令的目标地址
   Operand src1, dest, src2;//src1、dest、src2：这些字段分别代表源操作数1、目的操作数和源操作数2。在不同的指令中，这些操作数可以是输入数据、用于存储结果的寄存器或是立即数等。这些字段的类型是Operand结构体，包含了操作数的类型、值和其他相关信息。
   int width;//width：指示当前指令操作的数据宽度。这对于确定如何处理操作数（如整数的加法、减法等操作）非常重要。
   rtlreg_t tmp_reg[4];//tmp_reg[4]：这是一个临时寄存器数组，可能用于指令执行过程中的临时存储。在复杂的指令执行过程中，可能需要临时保存数据或中间结果。
   ISADecodeInfo isa;//isa：这个字段包含了指令集架构（Instruction Set Architecture，ISA）特定的解码信息。ISA定义了CPU支持的指令和指令的格式，这个字段可能用来存储与特定ISA相关的额外信息，比如指令的扩展操作码、特定的控制标志等。
} DecodeExecState;
/*DecodeExecState 结构体用于描述解码执行的状态，包括操作码、顺序
执行的PC（程序计数器）、是否是跳转指令、跳转的目标PC、源操作数和目
标操作数、指令宽度、临时寄存器数组，以及ISA特定的解码信息。*/

#define def_DHelper(name) void concat(decode_, name) (DecodeExecState *s)

#ifdef DEBUG
#define print_Dop(...) snprintf(__VA_ARGS__)
#else
#define print_Dop(...)
#endif

#endif
