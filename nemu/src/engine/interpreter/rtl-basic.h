#ifndef __RTL_BASIC_H__
#define __RTL_BASIC_H__

#include "c_op.h"
#include <memory/vaddr.h>

/* RTL basic instructions */

#define def_rtl_compute_reg(name) \
  static inline def_rtl(name, rtlreg_t* dest, const rtlreg_t* src1, const rtlreg_t* src2) { \
    *dest = concat(c_, name) (*src1, *src2); \
  }

#define def_rtl_compute_imm(name) \
  static inline def_rtl(name ## i, rtlreg_t* dest, const rtlreg_t* src1, const sword_t imm) { \
    *dest = concat(c_, name) (*src1, imm); \
  }

#define def_rtl_compute_reg_imm(name) \
  def_rtl_compute_reg(name) \
  def_rtl_compute_imm(name) \

// compute

def_rtl_compute_reg_imm(add)
def_rtl_compute_reg_imm(sub)
def_rtl_compute_reg_imm(and)
def_rtl_compute_reg_imm(or )
def_rtl_compute_reg_imm(xor)
def_rtl_compute_reg_imm(shl)
def_rtl_compute_reg_imm(shr)
def_rtl_compute_reg_imm(sar)

#ifdef ISA64
def_rtl_compute_reg_imm(addw)
def_rtl_compute_reg_imm(subw)
def_rtl_compute_reg_imm(shlw)
def_rtl_compute_reg_imm(shrw)
def_rtl_compute_reg_imm(sarw)
#define rtl_addiw rtl_addwi
#define rtl_shliw rtl_shlwi
#define rtl_shriw rtl_shrwi
#define rtl_sariw rtl_sarwi
#endif
//注意开头没有#define
static inline def_rtl(setrelop, uint32_t relop, rtlreg_t* dest,//setrelop:set relational operation
   const rtlreg_t* src1, const rtlreg_t* src2) {//setrelop意味着"设置关系操作的结果"，即根据两个操作数之间的关系比较结果来设置一个目标寄存器的值
   *dest = interpret_relop(relop, *src1, *src2);
}

static inline def_rtl(setrelopi, uint32_t relop, rtlreg_t* dest,
   const rtlreg_t* src1, sword_t imm) {
   *dest = interpret_relop(relop, *src1, imm);
}

// mul/div

def_rtl_compute_reg(mul_lo)
def_rtl_compute_reg(mul_hi)
def_rtl_compute_reg(imul_lo)
def_rtl_compute_reg(imul_hi)
def_rtl_compute_reg(div_q)
def_rtl_compute_reg(div_r)
def_rtl_compute_reg(idiv_q)
def_rtl_compute_reg(idiv_r)

#ifdef ISA64
def_rtl_compute_reg(mulw)
def_rtl_compute_reg(divw)
def_rtl_compute_reg(divuw)
def_rtl_compute_reg(remw)
def_rtl_compute_reg(remuw)
#endif

static inline def_rtl(div64_q, rtlreg_t* dest,//div64_q: q: quotient (math.)
   const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
   uint64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);//通过将src1_hi的值左移32位并与src1_lo的值进行按位或操作，构造出64位的被除数 
   uint32_t divisor = (*src2);//然后使用这个被除数除以src2指向的32位无符号除数，计算出商，并将结果存储在dest指向的寄存器中 
   *dest = dividend / divisor;
}

static inline def_rtl(div64_r, rtlreg_t* dest,//div64_r: r: residue (math.)
   const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
   uint64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
   uint32_t divisor = (*src2);
   *dest = dividend % divisor;
}

static inline def_rtl(idiv64_q, rtlreg_t* dest,
   const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
   int64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
   int32_t divisor = (*src2);
   *dest = dividend / divisor;
}

static inline def_rtl(idiv64_r, rtlreg_t* dest,
   const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
   int64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
   int32_t divisor = (*src2);
   *dest = dividend % divisor;
}

// memory
//`rtl_lm`, `rtl_lms` 和 `rtl_sm` 指的是访问guest（被模拟的系统）内存的操作，分别对应于加载内存、加载并修改内存、存储内存。
//lm（Load Memory）加载内存
static inline def_rtl(lm, rtlreg_t* dest, const rtlreg_t* addr, word_t offset, int len) {
   *dest = vaddr_read(*addr + offset, len);
}
//sm（Store Memory）存储内存
static inline def_rtl(sm, const rtlreg_t* addr, word_t offset, const rtlreg_t* src1, int len) {
   vaddr_write(*addr + offset, *src1, len);
}
//lms（Load Memory with Sign-extension）加载并修改内存
static inline def_rtl(lms, rtlreg_t* dest, const rtlreg_t* addr, word_t offset, int len) {
   word_t val = vaddr_read(*addr + offset, len);
   switch (len) {
   case 4: *dest = (sword_t)(int32_t)val; return;
   case 1: *dest = (sword_t)(int8_t)val; return;
   case 2: *dest = (sword_t)(int16_t)val; return;
   default: assert(0);
   }
}
//`rtl_host_lm` 和 `rtl_host_sm` 指的是访问host（模拟器运行的系统）内存的操作。
//host_lm（Host Load Memory）
static inline def_rtl(host_lm, rtlreg_t* dest, const void* addr, int len) {
   switch (len) {
   case 4: *dest = *(uint32_t*)addr; return;
   case 1: *dest = *(uint8_t*)addr; return;
   case 2: *dest = *(uint16_t*)addr; return;
   default: assert(0);
   }
}
//host_sm（Host Store Memory）
static inline def_rtl(host_sm, void* addr, const rtlreg_t* src1, int len) {
   switch (len) {
   case 4: *(uint32_t*)addr = *src1; return;
   case 1: *(uint8_t*)addr = *src1; return;
   case 2: *(uint16_t*)addr = *src1; return;
   default: assert(0);
   }
}

// control
//跳转, 包括直接跳转rtl_j, 间接跳转rtl_jr和条件跳转rtl_jrelop
static inline def_rtl(j, vaddr_t target) {//rtl_j 表示“Register Transfer Language Jump”
   s->jmp_pc = target;//这种类型的跳转不依赖于任何条件，执行时直接将程序的控制流跳转到指定的地址。
   s->is_jmp = true;
}

static inline def_rtl(jr, rtlreg_t* target) {//“jump register”（寄存器跳转）
   s->jmp_pc = *target;//一种间接跳转指令，意味着跳转的目的地（地址）存储在一个寄存器中，而不是直接在指令中给出。
   s->is_jmp = true;
}

static inline def_rtl(jrelop, uint32_t relop,//jrelop代表“jump relational operator”（跳转关系操作符）
   const rtlreg_t* src1, const rtlreg_t* src2, vaddr_t target) {
   bool is_jmp = interpret_relop(relop, *src1, *src2);
   if (is_jmp) rtl_j(s, target);
}
#endif
