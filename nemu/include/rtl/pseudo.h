#ifndef __RTL_PSEUDO_H__
#define __RTL_PSEUDO_H__

#ifndef __RTL_RTL_H__
#error "Should be only included by <rtl/rtl.h>"
#endif

/* RTL pseudo instructions */ //RTL 伪(编译)指令

static inline def_rtl(li, rtlreg_t* dest, const rtlreg_t imm) {//li (Load Immediate): dest <- imm将一个立即数（常数值）加载到一个寄存器中。通过使用 rtl_addi 指令将零寄存器 (rz) 与立即数相加来实现。
   rtl_addi(s, dest, rz, imm);// dest <- imm
}

static inline def_rtl(mv, rtlreg_t* dest, const rtlreg_t* src1) {//mv (move): dest <- src1
   if (dest != src1) rtl_add(s, dest, src1, rz);//dest <- src1
}

static inline def_rtl(not, rtlreg_t* dest, const rtlreg_t* src1) {//not (?bitwise NOT operator): dest <- ~src1
   // dest <- ~src1
   // TODO();
   *s0 = ~(*src1); // 创建临时变量来存储取反结果
   rtl_add(s, dest, s0, rz);
}

static inline def_rtl(neg, rtlreg_t* dest, const rtlreg_t* src1) {//neg (take a negative number): dest <- -src1
   // dest <- -src1
   //TODO();
   *s0 = -(*src1);
   rtl_add(s, dest, s0, rz);
}

static inline def_rtl(sext, rtlreg_t* dest, const rtlreg_t* src1, int width) {//sext (Sign Extension): 符号扩展 dest <- signext(src1[(width * 8 - 1) .. 0])
   // dest <- signext(src1[(width * 8 - 1) .. 0])
   //TODO();
   *s0 = 32 - width * 8; // 计算需要左移然后右移的位数
   // 先将src1左移，丢弃高位，然后算术右移回来，进行符号扩展
   *dest = (rtlreg_t)(((int32_t)(*src1) << *s0) >> *s0);
}

static inline def_rtl(zext, rtlreg_t* dest, const rtlreg_t* src1, int width) {//zext (Zero Extension):零扩展 dest <- zeroext(src1[(width * 8 - 1) .. 0])
   // dest <- zeroext(src1[(width * 8 - 1) .. 0])
   //TODO();
   // 创建一个掩码，它在低 width 位为 1，其他位为 0。
   *s0 = (1ULL << (width * 8)) - 1;
   // 将 src1 的值与掩码进行 AND 操作，然后赋值给 dest。
   *dest = *src1 & *s0;
}

static inline def_rtl(msb, rtlreg_t* dest, const rtlreg_t* src1, int width) {//msb (Most Significant Bit):用于获取一个数字中最重要的位（即最左边的位） dest <- src1[width * 8 - 1] 
   // dest <- src1[width * 8 - 1]
   //TODO();
   // 由于rtlreg_t是无符号32位整型，所以这里不需要担心符号扩展的问题
   // 计算位移量，以便将感兴趣的MSB位移到最低位
   *s0 = 8 * width - 1;

   // 将src1右移，使目标MSB位于最低位，然后与1进行逻辑与操作
   // 这样可以提取出MSB的值(最重要的位--最高位--最右边的)
   *dest = (*src1 >> *s0) & 1;
}

#endif
