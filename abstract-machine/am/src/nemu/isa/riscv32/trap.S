#define concat_temp(x, y) x ## y   
# 连接两个宏参数，不进行展开
#define concat(x, y) concat_temp(x, y) 
# 实际用于连接两个宏参数，通过另一个宏间接实现，以确保参数先展开

#define MAP(c, f) c(f)             
# 将函数宏f应用于集合宏c，实现函数的批量调用

#define REGS(f) \
      f( 1)       f( 3) f( 4) f( 5) f( 6) f( 7) f( 8) f( 9) \
f(10) f(11) f(12) f(13) f(14) f(15) f(16) f(17) f(18) f(19) \
f(20) f(21) f(22) f(23) f(24) f(25) f(26) f(27) f(28) f(29) \
f(30) f(31)                        # 定义一个包含大部分通用寄存器的列表，对每个寄存器应用宏f

#define PUSH(n) sw concat(x, n), (n * 4)(sp); 
# 定义PUSH宏，将寄存器xn的内容存储到堆栈sp指定的偏移处
#define POP(n)  lw concat(x, n), (n * 4)(sp); 
# 定义POP宏，从堆栈sp指定的偏移处加载内容到寄存器xn

#define CONTEXT_SIZE ((32 + 3) * 4) 
# 定义保存完整上下文所需的总字节数，包括32个通用寄存器和3个特殊寄存器
#define OFFSET_SP     ( 2 * 4)      
# 定义堆栈指针sp在保存的上下文中的偏移
#define OFFSET_CAUSE  (32 * 4)      
# 定义异常原因寄存器scause在保存的上下文中的偏移
#define OFFSET_STATUS (33 * 4)      
# 定义状态寄存器sstatus在保存的上下文中的偏移
#define OFFSET_EPC    (34 * 4)      
# 定义异常程序计数器sepc在保存的上下文中的偏移


.globl __am_asm_trap         # 声明 __am_asm_trap 为全局符号，使得其他模块可以引用
__am_asm_trap:               # 定义异常处理程序的开始
  addi sp, sp, -CONTEXT_SIZE # 扩展堆栈空间以保存当前的寄存器上下文

  MAP(REGS, PUSH)            # 使用宏将所有需要保存的寄存器推入堆栈

  mv t0, sp                  # 将当前的堆栈指针保存
  addi t0, t0, CONTEXT_SIZE  # 计算调整后的堆栈指针位置
  sw t0, OFFSET_SP(sp)       # 将调整后的堆栈指针保存在堆栈中

  csrr t0, scause            # 读取异常原因寄存器
  csrr t1, sstatus           # 读取状态寄存器
  csrr t2, sepc              # 读取异常程序计数器
  sw t0, OFFSET_CAUSE(sp)    # 将异常原因保存到堆栈
  sw t1, OFFSET_STATUS(sp)   # 将状态寄存器保存到堆栈
  sw t2, OFFSET_EPC(sp)      # 将异常程序计数器保存到堆栈

  mv a0, sp                  # 将当前的堆栈指针传递给 a0，作为参数传递给中断处理函数
  jal __am_irq_handle        # 调用中断处理函数 __am_irq_handle

  lw t1, OFFSET_STATUS(sp)   # 从堆栈恢复状态寄存器
  lw t2, OFFSET_EPC(sp)      # 从堆栈恢复异常程序计数器
  csrw sstatus, t1           # 将状态寄存器写回
  csrw sepc, t2              # 将异常程序计数器写回

  MAP(REGS, POP)             # 使用宏从堆栈恢复所有之前保存的寄存器

  addi sp, sp, CONTEXT_SIZE  # 恢复堆栈指针到异常处理前的位置

  sret                       # 从异常处理程序返回到中断或异常发生前的代码位置
