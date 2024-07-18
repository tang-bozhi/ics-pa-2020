// //system.h 可能包含系统调用和特权指令，这些指令与操作系统的交互和硬件控制有关。例如：
// ECALL, EBREAK
// CSRRW, CSRRS, CSRRC（控制状态寄存器读写指令）
// 指令来操控特权级状态和中断管理

// 读取 CSR 寄存器
void csr_read(rtlreg_t* dest, int csr) {
   switch (csr) {
   case 0x142:  // scause 寄存器地址
      *dest = cpu.csr.scause;
      break;
   case 0x141:  // sepc 寄存器地址
      *dest = cpu.csr.sepc;
      break;
   case 0x180:  // sstatus 寄存器地址
      *dest = cpu.csr.sstatus;
      break;
   case 0x100:  // stvec 寄存器地址
      *dest = cpu.csr.stvec;
      break;
   default:
      // 处理未知的 CSR 寄存器访问
      *dest = 0; // 或者设置一个特定的错误代码
      printf("Attempted to read unknown CSR 0x%X\n", csr);
      break;
   }
}

// 写入 CSR 寄存器
void csr_write(int csr, rtlreg_t src) {
   switch (csr) {
   case 0x142:  // scause 寄存器地址
      cpu.csr.scause = src;
      break;
   case 0x141:  // sepc 寄存器地址
      cpu.csr.sepc = src;
      break;
   case 0x180:  // sstatus 寄存器地址
      cpu.csr.sstatus = src;
      break;
   case 0x100:  // stvec 寄存器地址
      cpu.csr.stvec = src;
      break;
   default:
      // 处理未知的 CSR 寄存器访问
      printf("Attempted to write to unknown CSR 0x%X\n", csr);
      break;
   }
}

// 原子读写 CSR
static inline def_EHelper(csrrw) {
   int csr = s->isa.instr.csr.csr;
   int rd = s->isa.instr.csr.rd;
   int rs1 = s->isa.instr.csr.rs1;

   // 读取源寄存器的值
   rtlreg_t rs1_val = rs1 != 0 ? *(&s->gpr[rs1]._32) : 0;

   // 读取当前CSR的值
   rtlreg_t csr_val;
   csr_read(&csr_val, csr);  // 假设csr_read已经定义，读取当前CSR值

   // 将源寄存器的值写入到CSR
   csr_write(csr, rs1_val);  // 假设csr_write已经定义，写入新的CSR值

   // 将旧的CSR值写入到目标寄存器
   if (rd != 0) {  // 如果目标寄存器不是零寄存器
      rtl_li(s, &s->gpr[rd]._32, csr_val);
   }

   // 打印指令执行的结果，用于调试
   print_asm("csrrw %s, %s, 0x%x", reg_name(rd), reg_name(rs1), csr);
}


// 原子读取并设置 CSR 中的位
static inline def_EHelper(csrrs) {
   // 提取 CSR 地址和寄存器编号
   int csr = s->isa.instr.csr.csr;
   int rd = s->isa.instr.csr.rd;
   int rs1 = s->isa.instr.csr.rs1;

   // 读取源寄存器的值
   rtlreg_t rs1_val = rs1 != 0 ? *(&s->gpr[rs1]._32) : 0;

   // 读取当前 CSR 的值
   rtlreg_t csr_val;
   csr_read(&csr_val, csr);

   // 只有当 rs1 不是零寄存器时才修改 CSR
   if (rs1 != 0) {
      rtlreg_t new_csr_val = csr_val | rs1_val;
      csr_write(csr, new_csr_val);
   }

   // 将旧的 CSR 值写入到目标寄存器，除非 rd 是 x0
   if (rd != 0) {
      rtl_li(s, &s->gpr[rd]._32, csr_val);
   }

   // 打印指令执行的结果，用于调试
   print_asm("csrrs %s, %s, 0x%x", reg_name(rd), reg_name(rs1), csr);
}


// 原子读取并清除 CSR 中的位
static inline def_EHelper(csrrc) {
   // 提取 CSR 地址和寄存器编号
   int csr = s->isa.instr.csr.csr;
   int rd = s->isa.instr.csr.rd;
   int rs1 = s->isa.instr.csr.rs1;

   // 读取源寄存器的值
   rtlreg_t rs1_val = rs1 != 0 ? *(&s->gpr[rs1]._32) : 0;

   // 读取当前 CSR 的值
   rtlreg_t csr_val;
   csr_read(&csr_val, csr);

   // 只有当 rs1 不是零寄存器时才修改 CSR
   if (rs1 != 0) {
      rtlreg_t new_csr_val = csr_val & ~rs1_val;
      csr_write(csr, new_csr_val);
   }

   // 将旧的 CSR 值写入到目标寄存器，除非 rd 是 x0
   if (rd != 0) {
      rtl_li(s, &s->gpr[rd]._32, csr_val);
   }

   // 打印指令执行的结果，用于调试
   print_asm("csrrc %s, %s, 0x%x", reg_name(rd), reg_name(rs1), csr);
}

//imm
static inline def_EHelper(csrrwi) {
   // 从解码信息中提取 CSR 地址
   int csr = s->isa.instr.csr.csr;
   // 获取目标寄存器的编号
   int rd = s->isa.instr.csr.rd;
   // 从 rs1 字段提取5位立即数
   unsigned uimm = s->isa.instr.csr.rs1;

   // 读取当前 CSR 的值
   rtlreg_t csr_val;
   csr_read(&csr_val, csr);

   // 使用从 rs1 字段提取的 5 位无符号立即数更新 CSR
   csr_write(csr, uimm);

   // 将旧的 CSR 值写入到目标寄存器，除非 rd 是 x0
   if (rd != 0) {
      rtl_li(s, &s->gpr[rd]._32, csr_val);
   }

   // 打印指令执行的结果，用于调试
   print_asm("csrrwi %s, 0x%x, 0x%x", reg_name(rd), uimm, csr);
}

// 原子读取并设置 CSR 中的位 (立即数)
static inline def_EHelper(csrrsi) {
   // 提取 CSR 地址和目标寄存器编号
   int csr = s->isa.instr.csr.csr;
   int rd = s->isa.instr.csr.rd;
   // 从 rs1 字段提取 5 位立即数
   unsigned uimm = s->isa.instr.csr.rs1;

   // 读取当前 CSR 的值
   rtlreg_t csr_val;
   csr_read(&csr_val, csr);

   // 只有当立即数不为零时才修改 CSR
   if (uimm != 0) {
      rtlreg_t new_csr_val = csr_val | uimm;
      csr_write(csr, new_csr_val);
   }

   // 将旧的 CSR 值写入到目标寄存器，除非 rd 是 x0
   if (rd != 0) {
      rtl_li(s, &s->gpr[rd]._32, csr_val);
   }

   // 打印指令执行的结果，用于调试
   print_asm("csrrsi %s, 0x%x, 0x%x", reg_name(rd), uimm, csr);
}

// 原子读取并清除 CSR 中的位 (立即数)
static inline def_EHelper(csrrci) {
   // 提取 CSR 地址和目标寄存器编号
   int csr = s->isa.instr.csr.csr;
   int rd = s->isa.instr.csr.rd;
   // 从 rs1 字段提取 5 位立即数
   unsigned uimm = s->isa.instr.csr.rs1;

   // 读取当前 CSR 的值
   rtlreg_t csr_val;
   csr_read(&csr_val, csr);

   // 只有当立即数不为零时才修改 CSR
   if (uimm != 0) {
      rtlreg_t new_csr_val = csr_val & ~uimm;
      csr_write(csr, new_csr_val);
   }

   // 将旧的 CSR 值写入到目标寄存器，除非 rd 是 x0
   if (rd != 0) {
      rtl_li(s, &s->gpr[rd]._32, csr_val);
   }

   // 打印指令执行的结果，用于调试
   print_asm("csrrci %s, 0x%x, 0x%x", reg_name(rd), uimm, csr);
}
