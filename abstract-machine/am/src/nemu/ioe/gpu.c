#include <am.h>
#include <nemu.h>

// 定义同步地址常量，控制屏幕更新
#define SYNC_ADDR (VGACTL_ADDR + 4)

// 初始化GPU
void __am_gpu_init() {
  // int i;
  // int w = inl(VGACTL_ADDR) >> 16;  // TODO: 获取正确的宽度
  // int h = (uint64_t)inw(VGACTL_ADDR);  // TODO: 获取正确的高度
  // uint32_t* fb = (uint32_t*)(uintptr_t)FB_ADDR;  // 指向帧缓冲区的指针
  // // 初始化帧缓冲区，为每个像素赋予一个唯一的颜色值
  // for (i = 0; i < w * h; i++) fb[i] = i;
  // outl(SYNC_ADDR, 1);  // 向同步地址写入1，触发屏幕更新
}

// 获取GPU配置
void __am_gpu_config(AM_GPU_CONFIG_T* cfg) {
  *cfg = (AM_GPU_CONFIG_T){
    .present = true, .has_accel = false,
    .width = inl(VGACTL_ADDR) >> 16, .height = inw(VGACTL_ADDR),
    .vmemsz = 0  // 视频内存大小，初始设置为0
  };
}

// 绘制到帧缓冲区
void __am_gpu_fbdraw(AM_GPU_FBDRAW_T* ctl) {
  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;  // 绘制的起始坐标和宽高
  if (!ctl->sync && (w == 0 || h == 0))
    return;  // 如果不需要同步且宽度或高度为零，则不执行绘制
  uint32_t* pixels = ctl->pixels;  // 指向绘图数据的指针
  uint32_t* fb = (uint32_t*)(uintptr_t)FB_ADDR;  // 指向帧缓冲区的指针
  uint32_t screen_w = inl(VGACTL_ADDR) >> 16;  // 获取屏幕宽度
  // 将像素数据复制到帧缓冲区
  for (int i = y; i < y + h; i++) {
    for (int j = x; j < x + w; j++) {
      fb[screen_w * i + j] = pixels[w * (i - y) + (j - x)]; // 计算每个像素的索引并赋值
    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);    // 如果需要同步，将同步地址的值置为1，触发屏幕更新
  }
}

// 获取GPU状态
void __am_gpu_status(AM_GPU_STATUS_T* status) {
  status->ready = true;  // 设置GPU状态为就绪
}
