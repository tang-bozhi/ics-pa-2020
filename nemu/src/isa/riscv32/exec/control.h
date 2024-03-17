// control.h 可能包含控制流相关的指令。这些指令通常改变程序的执行流，如跳转指令和分支指令。例如：
// 分支指令：BEQ, BNE, BLT, BGE, BLTU, BGEU
// 跳转指令：JAL, JALR
// 条件分支指令：具体的比较和分支操作

static inline def_EHelper(jal) {
   // 将下一条指令的地址（PC + 4）保存到 rd 寄存器中，除非 rd 是 x0
   if (ddest != 0) {
      rtl_li(s, ddest, s->seq_pc + 4);
   }

   // 计算跳转目标地址并更新 PC
   rtl_j(s, s->seq_pc + imm);

   // 格式化打印此指令，用于调试
   print_asm_template2(jal);
}


static inline def_EHelper(jalr) {

}