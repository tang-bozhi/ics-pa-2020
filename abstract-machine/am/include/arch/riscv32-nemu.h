#ifndef ARCH_H__
#define ARCH_H__

// struct Context {
//   uintptr_t epc, cause, gpr[32], status;
//   void *pdir;
// };
//下方是按照教案需求照trap.S调整了的Context
struct Context {
  uintptr_t gpr[32]; // 通用寄存器（x1 到 x31）
  uintptr_t cause;   // cause
  uintptr_t status;  // status
  uintptr_t epc;     // epc
  void* pdir;        // 页目录指针，保持最后//需要注意的是, 虽然我们目前暂时不使用上文提到的地址空间信息, 但你在重新组织Context结构体时仍然需要正确地处理地址空间信息的位置, 否则你可能会在PA4中遇到难以理解的错误.
};


#define GPR1 gpr[17] // a7
#define GPR2 gpr[10]  // a0    //GPR2-4-x是对应navy-apps/libs/libos/src/syscall.c当中的GPR?
#define GPR3 gpr[11]  // a1
#define GPR4 gpr[12]  // a2
#define GPRx gpr[10]  // a0

#endif
