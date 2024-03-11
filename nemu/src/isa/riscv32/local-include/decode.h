#include <cpu/exec.h>
#include "rtl.h"

// decode operand helper
#define def_DopHelper(name) \
  void concat(decode_op_, name) (DecodeExecState *s, Operand *op, word_t val, bool load_val)

static inline def_DopHelper(i) {
   op->type = OP_TYPE_IMM;//这里是设置Oprand的种类,这将使得指令对应到其OP_TYPE_ 上 
   op->imm = val;//将指令放入,这里就已经用union的特性使指令的相应部分得到其应有的意义 

   print_Dop(op->str, OP_STR_SIZE, "%d", op->imm);
}

static inline def_DopHelper(r) {
   op->type = OP_TYPE_REG;
   op->reg = val;
   op->preg = &reg_l(val);

   print_Dop(op->str, OP_STR_SIZE, "%s", reg_name(op->reg));
}

static inline def_DHelper(I) {
   decode_op_r(s, id_src1, s->isa.instr.i.rs1, true);//这里的bool表示是否加载这个值
   decode_op_i(s, id_src2, s->isa.instr.i.simm11_0, true);
   decode_op_r(s, id_dest, s->isa.instr.i.rd, false);
}

static inline def_DHelper(U) {
   decode_op_i(s, id_src1, s->isa.instr.u.imm31_12 << 12, true);
   decode_op_r(s, id_dest, s->isa.instr.u.rd, false);

   print_Dop(id_src1->str, OP_STR_SIZE, "0x%x", s->isa.instr.u.imm31_12);
}

static inline def_DHelper(S) {
   decode_op_r(s, id_src1, s->isa.instr.s.rs1, true);
   sword_t simm = (s->isa.instr.s.simm11_5 << 5) | s->isa.instr.s.imm4_0;
   decode_op_i(s, id_src2, simm, true);
   decode_op_r(s, id_dest, s->isa.instr.s.rs2, true);
}
