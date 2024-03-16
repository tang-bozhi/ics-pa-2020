// compute.h 可能包含算术和逻辑计算相关的指令。由于这些指令通常涉及对寄存器中的数据进行操作，不涉及内存访问。例如：
// 算术指令：ADD, SUB, SLT, SLTU, XOR, OR, AND（如图片所示）
// 移位指令：SLL, SRL, SRA
// 立即数指令：ADDI, SLTI, SLTIU, XORI, ORI, ANDI
// 算术扩展指令：SLLI, SRLI, SRAI

//lui（Load Upper Immediate）LUI(加载高位立即数)被用于构建 32 位常量 它将一个 20 位的立即数加载到寄存器的高 20 位,同时将低 12 位清零.
static inline def_EHelper(lui) {
   rtl_li(s, ddest, id_src1->imm);//def_DHelper(U)中已经: s->isa.instr.u.imm31_12 << 12
   print_asm_template2(lui);
}

//AUIPC(加高位立即数到 pc)被用于构建 pc 相对地址它使用 U 类型格式。AUIPC 根据 U 立即数形成 32 位偏移量(最低 12 位填零)，把这个偏移量加到 AUIPC 指令的地址，然后把结 果放在寄存器 rd 中。
static inline def_EHelper(auipc) {
   rtl_li(s, ddest, s->seq_pc + id_src1->imm);
   print_asm_template2(auipc);
}

