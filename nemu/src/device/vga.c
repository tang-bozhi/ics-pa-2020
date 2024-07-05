#include <common.h>

#ifdef HAS_IOE

// 定义屏幕显示和分辨率配置
#define SHOW_SCREEN
//#define MODE_800x600

#ifdef MODE_800x600
# define SCREEN_W 800
# define SCREEN_H 600
#else
# define SCREEN_W 400
# define SCREEN_H 300
#endif
#define SCREEN_SIZE ((SCREEN_H * SCREEN_W) * sizeof(uint32_t))

#include <device/map.h>
#include <SDL2/SDL.h>

// 定义视频内存的基础虚拟内存地址
#define VMEM 0xa0000000

// VGA控制端口和内存映射I/O地址
#define VGACTL_PORT 0x100 // 注意这不是标准地址
#define VGACTL_MMIO 0xa1000100

static SDL_Renderer* renderer = NULL;
static SDL_Texture* texture = NULL;

// 指向视频内存数组的指针
static uint32_t(*vmem)[SCREEN_W] = NULL;
// VGA控制端口的基础指针
static uint32_t* vgactl_port_base = NULL;

// 更新屏幕的函数；仅当定义了SHOW_SCREEN时启用
static inline void update_screen() {
#ifdef SHOW_SCREEN
  // 使用视频内存更新纹理，然后渲染并显示
  SDL_UpdateTexture(texture, NULL, vmem, SCREEN_W * sizeof(vmem[0][0]));
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
#endif
}

// 根据VGA同步寄存器的条件调用更新屏幕的函数  
void vga_update_screen() {
  // 检查同步寄存器的值（保存在 vgactl_port_base[1] 中）
  if (vgactl_port_base[1] != 0) {
    // 当同步寄存器非零时，调用 update_screen() 更新屏幕
    update_screen();
    // 更新完毕后，将同步寄存器清零
    vgactl_port_base[1] = 0;
  }
}

// 初始化VGA设备，设置SDL和内存映射
void init_vga() {
#ifdef SHOW_SCREEN
  SDL_Window* window = NULL;
  char title[128];
  // 准备窗口标题
  sprintf(title, "%s-NEMU", str(__ISA__));
  SDL_Init(SDL_INIT_VIDEO);
  // 根据定义的分辨率创建窗口和渲染器
#ifdef MODE_800x600
  SDL_CreateWindowAndRenderer(SCREEN_W, SCREEN_H, 0, &window, &renderer);
#else
  // 在默认模式下，窗口大小放大2倍
  SDL_CreateWindowAndRenderer(SCREEN_W * 2, SCREEN_H * 2, 0, &window, &renderer);
#endif
  // 设置窗口标题
  SDL_SetWindowTitle(window, title);
  // 创建用于渲染的纹理
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STATIC, SCREEN_W, SCREEN_H);
#endif

  // 为VGA控制端口分配空间并设置屏幕尺寸
  vgactl_port_base = (void*)new_space(8);
  vgactl_port_base[0] = ((SCREEN_W) << 16) | (SCREEN_H);
  // 将VGA控制端口和MMIO映射到内存
  add_pio_map("screen", VGACTL_PORT, (void*)vgactl_port_base, 8, NULL);
  add_mmio_map("screen", VGACTL_MMIO, (void*)vgactl_port_base, 8, NULL);

  // 为视频内存分配空间并进行映射
  vmem = (void*)new_space(SCREEN_SIZE);
  add_mmio_map("vmem", VMEM, (void*)vmem, SCREEN_SIZE, NULL);
}
#endif	/* HAS_IOE */
