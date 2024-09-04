#ifndef __COMMON_H__
#define __COMMON_H__

//#define DEBUG
//#define DIFF_TEST

#ifdef __ISA_riscv64__
#define ISA64
#endif

#if _SHARE
// do not enable these features while building a reference design
#undef DIFF_TEST
#undef DEBUG
#endif

/* You will define this macro in PA2 */
#define HAS_IOE // 你会看到运行NEMU时会弹出一个新窗口, 用于显示VGA的输出(见下文)

#include <stdint.h>
#include <assert.h>
#include <string.h>

typedef uint8_t bool;

#ifdef ISA64
typedef uint64_t word_t;
typedef int64_t sword_t;
#define FMT_WORD "0x%016lx"
#else
typedef uint32_t word_t;
typedef int32_t sword_t; // s: signed
#define FMT_WORD "0x%08x" //"word format" 字格式
#endif

typedef word_t rtlreg_t;
typedef word_t vaddr_t;
typedef uint32_t paddr_t;
typedef uint16_t ioaddr_t;

#define false 0
#define true 1

#include <debug.h>
#include <macro.h>

#endif
