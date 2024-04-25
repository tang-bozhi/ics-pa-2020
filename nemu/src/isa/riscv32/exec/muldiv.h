//add和sub在nemu/src/isa/riscv32/exec/compute.h中
static inline def_EHelper(mul) {
   rtl_mul_lo(s, ddest, dsrc1, dsrc2);  // 将 src1 和 src2 相乘，结果存入 dest（低半部分）
   print_asm_template3(mul);
}

static inline def_EHelper(mulh) {
   rtl_imul_hi(s, ddest, dsrc1, dsrc2);  // 将 src1 和 src2 相乘，结果存入 dest（高半部分）
   print_asm_template3(mulh);
}

static inline def_EHelper(mulhsu) {
   // 特殊情况：将有符号的 src1 与无符号的 src2 相乘，高半部分结果存入 dest
   rtl_mulhsu_hi(s, ddest, dsrc1, dsrc2);
   print_asm_template3(mulhsu);
}

static inline def_EHelper(mulhu) {
   rtl_mul_hi(s, ddest, dsrc1, dsrc2);  // 无符号将 src1 和 src2 相乘，结果存入 dest（高半部分）
   print_asm_template3(mulhu);
}

static inline def_EHelper(div) {
   rtl_div_q(s, ddest, dsrc1, dsrc2);  // 将 src1 除以 src2，商存入 dest
   print_asm_template3(div);
}

static inline def_EHelper(divu) {
   rtl_div_q(s, ddest, dsrc1, dsrc2);  // 无符号将 src1 除以 src2，商存入 dest
   print_asm_template3(divu);
}

static inline def_EHelper(rem) {
   rtl_div_r(s, ddest, dsrc1, dsrc2);  // 将 src1 除以 src2，余数存入 dest
   print_asm_template3(rem);
}

static inline def_EHelper(remu) {
   rtl_div_r(s, ddest, dsrc1, dsrc2);  // 无符号将 src1 除以 src2，余数存入 dest
   print_asm_template3(remu);
}

