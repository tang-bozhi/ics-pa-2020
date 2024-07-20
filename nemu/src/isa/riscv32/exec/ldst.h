static inline def_EHelper(ld) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, s->width);

  print_Dop(id_src1->str, OP_STR_SIZE, "%d(%s)", id_src2->imm, reg_name(id_src1->reg));

  switch (s->isa.instr.i.funct3) {
  case 0:*ddest = (int32_t)(int8_t)*ddest; print_asm_template2(lb); break;
  case 1:*ddest = (int32_t)(int16_t)*ddest; print_asm_template2(lh); break;
  case 2:*ddest = (int32_t)(int32_t)*ddest; print_asm_template2(lw); break;
  case 4:*ddest = (uint32_t)(uint8_t)*ddest; print_asm_template2(lbu); break;
  case 5:*ddest = (uint32_t)(uint16_t)*ddest; print_asm_template2(lhu); break;
  }
}

static inline def_EHelper(st) {
  rtl_sm(s, dsrc1, id_src2->imm, ddest, s->width);

  print_Dop(id_src1->str, OP_STR_SIZE, "%d(%s)", id_src2->imm, reg_name(id_src1->reg));
  switch (s->width) {
  case 4: print_asm_template2(sw); break;
  case 2: print_asm_template2(sh); break;
  case 1: print_asm_template2(sb); break;
  default: assert(0);
  }
}
//已被教案完成 
