#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000//0b 1000 0000 0000 0000: 这个掩码用于检测键码的最高位（bit），如果该位为 1，则表示有键被按下

void __am_input_keybrd(AM_INPUT_KEYBRD_T* kbd) {
  uint32_t kc = inl(KBD_ADDR);
  kbd->keydown = kc & KEYDOWN_MASK ? true : false;
  kbd->keycode = kc & ~KEYDOWN_MASK
}
