// //system.h 可能包含系统调用和特权指令，这些指令与操作系统的交互和硬件控制有关。例如：
// ECALL, EBREAK
// CSRRW, CSRRS, CSRRC（控制状态寄存器读写指令）
// 指令来操控特权级状态和中断管理


static vaddr_t* csr_register(word_t imm) {
   switch (imm) {
   case 0x142: return &(cpu.csr.scause);   // scause
   case 0x100: return &(cpu.csr.sstatus);  // sstatus
   case 0x141: return &(cpu.csr.sepc);     // sepc
   case 0x105: return &(cpu.csr.stvec);    // stvec
   default: panic("Unknown csr");
   }
}

//#define ECALL(dnpc) { bool success; dnpc = ((isa_reg_str2val("a7", &success), s->pc)); }
#define _CSR_(i) *csr_register(i)

void raise_intr(DecodeExecState* s, word_t NO, vaddr_t epc);
static inline def_EHelper(ecall) {
   // 调用 raise_intr 函数，保存当前PC并设置异常号

   printf("调用ecall\n");
   raise_intr(s, cpu.gpr[17]._32, s->seq_pc);//s->gpr[17]: a7

   // 打印指令执行的结果，用于调试
   print_asm("ecall");
}

static inline def_EHelper(sret) {
   // 设置跳转标志和跳转地址
   s->is_jmp = 1;
   s->jmp_pc = cpu.csr.sepc;
   // 从控制状态寄存器中恢复程序计数器和其他状态
   //cpu.pc = cpu.csr.sepc; // 将 sepc 中保存的地址恢复到 pc 中
   print_asm("sret");
}

// static inline def_EHelper(mret) {
//    // 从控制状态寄存器中恢复程序计数器和其他状态
//    rtl_mv(s, &cpu.pc, &cpu.csr.mepc); // 将 mepc 中保存的地址恢复到 pc 中
//    rtl_li(s, &cpu.csr.mstatus, cpu.csr.mstatus & ~0x1800); // 清除 MPP 位，表示返回到用户模式

//    print_asm("mret");
// }

// 原子读写 CSR
static inline def_EHelper(csrrw) {
   //int rs1 = *dsrc1;
   //int csr = id_src2->imm;
   //int rd = *ddest;
   *ddest = _CSR_(id_src2->imm);
   _CSR_(id_src2->imm) = *dsrc1;

   // 打印指令执行的结果，用于调试
   print_asm_template3(csrrw);
}


// 原子读取并设置 CSR 中的位
static inline def_EHelper(csrrs) {
   //int rs1 = *dsrc1;
   //int csr = id_src2->imm;
   //int rd = *ddest;
   *ddest = _CSR_(id_src2->imm);
   _CSR_(id_src2->imm) |= *dsrc1;

   // 打印指令执行的结果，用于调试
   print_asm_template3(csrrs);
}


// 原子读取并清除 CSR 中的位
static inline def_EHelper(csrrc) {
   //int rs1 = *dsrc1;
   //int csr = id_src2->imm;
   //int rd = *ddest;
   *ddest = _CSR_(id_src2->imm);
   _CSR_(id_src2->imm) = (_CSR_(id_src2->imm) & ~(*dsrc1));

   // 打印指令执行的结果，用于调试
   print_asm_template3(csrrc);
}

//imm
static inline def_EHelper(csrrwi) {
   //int rs1 = *dsrc1;
   //int csr = id_src2->imm;
   //int rd = *ddest;
   *ddest = _CSR_(id_src2->imm);
   _CSR_(id_src2->imm) = *dsrc1;

   // 打印指令执行的结果，用于调试
   print_asm_template3(csrrwi);
}

// 原子读取并设置 CSR 中的位 (立即数)
static inline def_EHelper(csrrsi) {
   //int rs1 = *dsrc1;
   //int csr = id_src2->imm;
   //int rd = *ddest;
   *ddest = _CSR_(id_src2->imm);
   _CSR_(id_src2->imm) |= *dsrc1;

   // 打印指令执行的结果，用于调试
   print_asm_template3(csrrsi);
}

// 原子读取并清除 CSR 中的位 (立即数)
static inline def_EHelper(csrrci) {
   //int rs1 = *dsrc1;
   //int csr = id_src2->imm;
   //int rd = *ddest;
   *ddest = _CSR_(id_src2->imm);
   _CSR_(id_src2->imm) = (_CSR_(id_src2->imm) & ~(*dsrc1));

   // 打印指令执行的结果，用于调试
   print_asm_template3(csrrci);
}
