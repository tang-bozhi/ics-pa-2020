#ifndef __CPU_EXEC_H__
#define __CPU_EXEC_H__

#include <isa.h>
#include <rtl/rtl.h>
#include <memory/vaddr.h>
#include <cpu/decode.h>

#define def_EHelper(name) void concat(exec_, name) (DecodeExecState *s)

// empty decode helper
#define decode_empty(s)

#define IDEXW(idx, id, ex, w) CASE_ENTRY(idx, concat(decode_, id), concat(exec_, ex), w)//生成switch语句中的一个case项，其中包括了设置执行宽度、调用解码函数和执行函数的代码
#define IDEX(idx, id, ex)     IDEXW(idx, id, ex, 0)//这是一个特化的IDEXW宏，用于定义没有宽度参数（或宽度为0）的指令
#define EXW(idx, ex, w)       IDEXW(idx, empty, ex, w)//这个宏用于定义没有专门解码步骤（或解码函数为空）的指令，只有执行步骤和宽度参数
#define EX(idx, ex)           EXW(idx, ex, 0)//这是EXW的一个特化形式，用于定义没有宽度参数的执行指令
#define EMPTY(idx)            EX(idx, inv)//这个宏用于定义一个“空”的或无效的指令

// set_width() is defined in src/isa/$isa/exec/exec.c
#define CASE_ENTRY(idx, id, ex, w) case idx: set_width(s, w); id(s); ex(s); break;
//idx (Index)：这是指令的索引或者编号, 生成switch语句中的一个case项
//id (Identifier for Decode)：这是解码函数的后缀部分
//ex (Identifier for Execute)：这是执行函数的后缀部分。
//w (Width)：这代表指令执行时的某种“宽度”参数，可能是指数据宽度或其他特定于架构的宽度概念。
//如IDEXW(0xb0, mov_I2r, mov, 1)展开之后结果为:case 0xb0: set_width(s, 1); decode_mov_I2r(s); exec_mov(s); break;

//vaddr_ifetch 函数的主要功能是根据请求的长度（len），从指定的虚拟地址（addr）中取出指令数据。这个函数通过内联（static inline）定义，以便在编译时直接嵌入到调用它的代码位置，从而减少函数调用的开销，提高效率。
static inline uint32_t instr_fetch(vaddr_t* pc, int len) {
  uint32_t instr = vaddr_ifetch(*pc, len);
#ifdef DEBUG
  uint8_t* p_instr = (void*)&instr;
  int i;
  for (i = 0; i < len; i++) {
    extern char log_bytebuf[];
    strcatf(log_bytebuf, "%02x ", p_instr[i]);
  }
#endif
  (*pc) += len;
  return instr;
}

static inline void update_pc(DecodeExecState* s) {
  cpu.pc = (s->is_jmp ? s->jmp_pc : s->seq_pc);
}

#ifdef DEBUG
#define print_asm(...) \
  do { \
    extern char log_asmbuf[]; \
    strcatf(log_asmbuf, __VA_ARGS__); \
  } while (0)
#else
#define print_asm(...)
#endif

#ifndef suffix_char
#define suffix_char(width) ' '
#endif

#define print_asm_template1(instr) \
  print_asm(str(instr) "%c %s", suffix_char(id_dest->width), id_dest->str)

#define print_asm_template2(instr) \
  print_asm(str(instr) "%c %s,%s", suffix_char(id_dest->width), id_src1->str, id_dest->str)

#define print_asm_template3(instr) \
  print_asm(str(instr) "%c %s,%s,%s", suffix_char(id_dest->width), id_src1->str, id_src2->str, id_dest->str)

#endif
