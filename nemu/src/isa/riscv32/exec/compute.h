// compute.h 可能包含算术和逻辑计算相关的指令。由于这些指令通常涉及对寄存器中的数据进行操作，不涉及内存访问。例如：
// 算术指令：ADD, SUB, SLT, SLTU, XOR, OR, AND（如图片所示）
// 移位指令：SLL, SRL, SRA
// 立即数指令：ADDI, SLTI, SLTIU, XORI, ORI, ANDI
// 算术扩展指令：SLLI, SRLI, SRAI
static inline def_EHelper(lui) {//lui（Load Upper Immediate）是一种类型的指令,它用于将一个 20 位的立即数加载到寄存器的高 20 位,同时将低 12 位清零.
   rtl_li(s, ddest, id_src1->imm);//def_DHelper(U)中已经: s->isa.instr.u.imm31_12 << 12
   print_asm_template2(lui);
}

static inline def_EHelper(auipc) {
   rtl_li(s, ddest, s.seq_pc + id_src1->imm);
   print_asm_template2(auipc);
}
