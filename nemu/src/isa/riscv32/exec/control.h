// control.h 可能包含控制流相关的指令。这些指令通常改变程序的执行流，如跳转指令和分支指令。例如：
// 分支指令：BEQ, BNE, BLT, BGE, BLTU, BGEU
// 跳转指令：JAL, JALR
// 条件分支指令：具体的比较和分支操作

static inline def_EHelper(jal) {
   // 将下一条指令的地址（PC + 4）保存到 rd 寄存器中，除非 rd 是 x0
   if (ddest != rz) {// 注意这里使用 rz 检查是否为 x0
      rtl_li(s, ddest, s->seq_pc + 4);
   }
   //不需要下面的步骤,def_DHelper(J)已经完成了设定s->jmp_pc和s->is_jmp // 计算跳转目标地址并更新 PC
   //rtl_j(s, s->seq_pc + imm);

   // 格式化打印此指令，用于调试
   print_asm_template2(jal);
}


static inline def_EHelper(jalr) {
   // 计算跳转目标地址并进行对齐（清零最低位）
   rtl_addi(s, s0, dsrc1, id_src2->imm); // s0 = *dsrc1 + id_src2->imm
   rtl_andi(s, s0, s0, ~1); // 对齐，清零最低位

   // 将下一条指令的地址保存到 rd 寄存器中，除非 rd 是 x0
   if (ddest != &rzero) { // 使用 &rzero 来检查是否为 x0
      rtl_li(s, ddest, s->seq_pc + 4);
   }

   // 更新 PC 为跳转目标地址
   rtl_j(s, *s0);

   // 格式化打印此指令，用于调试
   print_asm_template2(jalr);
}

