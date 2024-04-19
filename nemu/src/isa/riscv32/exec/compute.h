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

//下方均为后来添加
//AUIPC(加高位立即数到 pc)被用于构建 pc 相对地址它使用 U 类型格式。AUIPC 根据 U 立即数形成 32 位偏移量(最低 12 位填零)，把这个偏移量加到 AUIPC 指令的地址，然后把结 果放在寄存器 rd 中。
static inline def_EHelper(auipc) {
   rtl_li(s, ddest, s->seq_pc + id_src1->imm);
   print_asm_template2(auipc);
}


static inline def_EHelper(addi) {
   rtl_addi(s, ddest, dsrc1, id_src2->imm);
   print_asm_template3(addi);
}

static inline def_EHelper(slti) {
   rtl_setrelopi(s, RELOP_LT, ddest, dsrc1, id_src2->imm);
   print_asm_template2(slti);
}

static inline def_EHelper(sltiu) {//由def_EHelper(slti)稍作修改得到
   rtl_setrelopi(s, RELOP_LTU, ddest, dsrc1, id_src2->imm);
   print_asm_template2(slti);
}

static inline def_EHelper(xori) {
   rtl_xori(s, ddest, dsrc1, id_src2->imm);
   print_asm_template2(xori);
}

static inline def_EHelper(ori) {
   rtl_ori(s, ddest, dsrc1, id_src2->imm);
   print_asm_template2(ori);
}

static inline def_EHelper(andi) {
   rtl_andi(s, ddest, dsrc1, id_src2->imm);
   print_asm_template2(andi);
}

static inline def_EHelper(srli) {//SRLI(Shift Logical Right Immediate)
   rtl_shri(s, ddest, dsrc1, ((id_src2->imm) & 3071));//去除func7,保留imm[4:0]:0b0100000
   print_asm_template2(andi);
}

static inline def_EHelper(slli) {//SLLI(Shift Logical Left Immediate)
   rtl_shli(s, ddest, dsrc1, ((id_src2->imm) & 3071));
   print_asm_template2(andi);
}

static inline def_EHelper(srai) {//SRAI(Shift Right Arithmetic Immediate)
   rtl_srai(s, ddest, dsrc1, ((id_src2->imm) & 3071));
   print_asm_template2(srai);
}

static inline def_EHelper(add) {
   rtl_add(s, ddest, dsrc1, dsrc2);
   print_asm_template3(add);
}

static inline def_EHelper(sub) {
   rtl_sub(s, ddest, dsrc1, dsrc2);
   print_asm_template3(sub);
}

static inline def_EHelper(sll) {//逻辑左移 
   rtl_setrelop(s, RELOP_LT, ddest, dsrc1, dsrc2);
   print_asm_template3(sll);
}

static inline def_EHelper(slt) {//set less than 
   rtl_setrelop(s, RELOP_LT, ddest, dsrc1, dsrc2);
   print_asm_template3(slt);
}

static inline def_EHelper(sltu) {//set less than unsigned
   rtl_setrelop(s, RELOP_LTU, ddest, dsrc1, dsrc2);
   print_asm_template3(sltu);
}

static inline def_EHelper(xor) {
   rtl_xor(s, ddest, dsrc1, dsrc2);
   print_asm_template3(xor);
}

static inline def_EHelper(srl) {
   rtl_shr(s, ddest, dsrc1, dsrc2);//手册中的逻辑右移是srl,这里把rtl_shr的名字更改了
   print_asm_template3(srl);
}

static inline def_EHelper(sra) {
   rtl_sra(s, ddest, dsrc1, dsrc2);
   print_asm_template3(sra);
}

static inline def_EHelper(or ) {
   rtl_or(s, ddest, dsrc1, dsrc2);
   print_asm_template3(or );
}

static inline def_EHelper(and) {
   rtl_and(s, ddest, dsrc1, dsrc2);
   print_asm_template3(and);
}