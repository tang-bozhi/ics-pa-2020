#include <cpu/exec.h>
#include "rtl.h"

// decode operand helper
#define def_DopHelper(name) \
  void concat(decode_op_, name) (DecodeExecState *s, Operand *op, word_t val, bool load_val)

static inline def_DopHelper(i) {
   op->type = OP_TYPE_IMM;//这里是设置Oprand的种类,这将使得指令对应到其OP_TYPE_ 上 
   op->imm = val;//将指令放入 这里有隐式类型转换 不用考虑op->imm是uint32_t可能和val不匹配的问题

   print_Dop(op->str, OP_STR_SIZE, "%d", op->imm);
}

static inline def_DopHelper(r) {
   op->type = OP_TYPE_REG;//同上 //这里是设置Oprand的种类,这将使得指令对应到其OP_TYPE_ 上 
   op->reg = val;//这里存储寄存器的编号。
   op->preg = &reg_l(val);//preg直接指向word_t val()  &reg_l(val)对相应寄存器取值 

   print_Dop(op->str, OP_STR_SIZE, "%s", reg_name(op->reg));
}

static inline def_DHelper(I) {
   decode_op_r(s, id_src1, s->isa.instr.i.rs1, true);//这里的bool表示是否加载这个值 (但是这里没有用到)
   decode_op_i(s, id_src2, s->isa.instr.i.simm11_0, true);
   decode_op_r(s, id_dest, s->isa.instr.i.rd, false);
}

static inline def_DHelper(U) {//Upper 上移
   decode_op_i(s, id_src1, s->isa.instr.u.imm31_12 << 12, true);
   decode_op_r(s, id_dest, s->isa.instr.u.rd, false);

   print_Dop(id_src1->str, OP_STR_SIZE, "0x%x", s->isa.instr.u.imm31_12);
}

static inline def_DHelper(S) {//Store 存储
   decode_op_r(s, id_src1, s->isa.instr.s.rs1, true);
   sword_t simm = (s->isa.instr.s.simm11_5 << 5) | s->isa.instr.s.imm4_0;
   decode_op_i(s, id_src2, simm, true);
   decode_op_r(s, id_dest, s->isa.instr.s.rs2, true);
}

//下方均为后来添加
static inline def_DHelper(R) {//Register
   // 解码源寄存器1
   decode_op_r(s, id_src1, s->isa.instr.r.rs1, true);
   // 解码源寄存器2
   decode_op_r(s, id_src2, s->isa.instr.r.rs2, true);
   // 解码目标寄存器
   decode_op_r(s, id_dest, s->isa.instr.r.rd, false);
}

static inline def_DHelper(B) {//Branch
   // 解码源寄存器1
   decode_op_r(s, id_src1, s->isa.instr.b.rs1, true);

   // 解码源寄存器2
   decode_op_r(s, id_src2, s->isa.instr.b.rs2, true);

   // B-type 指令立即数字段的位组合
   *s0 = (
      ((s->isa.instr.b.imm11) << 11) |
      ((s->isa.instr.b.imm10_5) << 5) |
      ((s->isa.instr.b.imm4_1) << 1) |
      ((s->isa.instr.b.imm12) << 12)
      );

   // 立即数字段进行符号扩展
   *s0 = (*s0 << 19) >> 19;

   //存放处理好了的立即数  "按照特定的位模式组合并进行符号扩展"了的立即数
   id_src2->imm = *s0;

   // 计算跳转目标地址（当前 PC 加上偏移量）
   s->jmp_pc = cpu.pc + *s0;//必须使用cpu.pc,s->seq_pc已经由instr_fetch中的(*pc) += len;指向了下一个指令

   // 设置分支标志
   s->is_jmp = 1;
}

static inline def_DHelper(J) {//Jump
   // 解码目标寄存器 
   decode_op_r(s, id_dest, s->isa.instr.j.rd, false);

   // J-type 指令立即数字段的位组合
   *s0 = (
      (s->isa.instr.j.imm20 << 20) |
      (s->isa.instr.j.imm19_12 << 12) |
      (s->isa.instr.j.imm11 << 11) |
      (s->isa.instr.j.imm10_1 << 1)
      );

   // 立即数字段进行符号扩展
   //*s0 = (*s0 << 11) >> 11;//当前环境不符合JAL要求:不能处理负数 imm20=0 
   // 对立即数进行符号扩展
   if (*s0 & (1 << 20)) { // 检查最高位（即imm20），如果为1，说明是负数
      *s0 |= 0xFFF00000; // 通过或操作将高12位设置为1，进行符号扩展
   }
   else {
      *s0 &= 0x001FFFFF; // 如果不是负数，确保只有21位有效，高位清零
   }

   //存放处理好了的立即数  "按照特定的位模式组合并进行符号扩展"了的立即数
   id_src2->imm = *s0;

   // 计算跳转目标地址（当前 PC 加上偏移量）
   s->jmp_pc = cpu.pc + *s0;

   // 设置跳转标志
   s->is_jmp = 1;
}

