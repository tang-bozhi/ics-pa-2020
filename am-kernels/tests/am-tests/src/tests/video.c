#include <amtest.h>

#define FPS 30 // 每秒帧数（frames per second）
#define N   32 // 画布大小为32x32

// 将RGB值转换为32位整数表示的像素值
static inline uint32_t pixel(uint8_t r, uint8_t g, uint8_t b) {
  return (r << 16) | (g << 8) | b;
}

// 从32位整数表示的像素值中提取R、G、B值
static inline uint8_t R(uint32_t p) { return p >> 16; }
static inline uint8_t G(uint32_t p) { return p >> 8; }
static inline uint8_t B(uint32_t p) { return p; }

static uint32_t canvas[N][N]; // 画布，用于存储每个像素的颜色值
static int used[N][N]; // 标记每个位置是否已被使用

static uint32_t color_buf[32 * 32]; // 缓冲区，用于存储要绘制的块的颜色值

// 绘制画布
void redraw() {
  int w = io_read(AM_GPU_CONFIG).width / N; // 计算每个块的宽度
  int h = io_read(AM_GPU_CONFIG).height / N; // 计算每个块的高度
  int block_size = w * h; // 每个块的像素数量
  assert((uint32_t)block_size <= LENGTH(color_buf)); // 确保缓冲区足够大

  int x, y, k;
  for (y = 0; y < N; y++) {
    for (x = 0; x < N; x++) {
      // 将画布中的颜色值复制到缓冲区中
      for (k = 0; k < block_size; k++) {
        color_buf[k] = canvas[y][x];
      }
      // 绘制缓冲区内容到屏幕
      io_write(AM_GPU_FBDRAW, x * w, y * h, color_buf, w, h, false);
    }
  }
  // 刷新屏幕
  io_write(AM_GPU_FBDRAW, 0, 0, NULL, 0, 0, true);
}

// 根据时间戳生成颜色值
static uint32_t p(int tsc) {
  int b = tsc & 0xff; // 取时间戳的低8位
  return pixel(b * 6, b * 7, b); // 生成颜色值
}

// 更新画布
void update() {
  static int tsc = 0; // 时间戳计数器
  static int dx[4] = { 0, 1, 0, -1 }; // 方向数组，用于控制螺旋前进
  static int dy[4] = { 1, 0, -1, 0 };

  tsc++; // 增加时间戳

  // 清空used数组
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      used[i][j] = 0;
    }
  }

  int init = tsc * 1; // 初始颜色值
  canvas[0][0] = p(init); used[0][0] = 1; // 初始化起点
  int x = 0, y = 0, d = 0; // 当前坐标和方向
  for (int step = 1; step < N * N; step++) {
    for (int t = 0; t < 4; t++) {
      int x1 = x + dx[d], y1 = y + dy[d]; // 计算下一个坐标
      if (x1 >= 0 && x1 < N && y1 >= 0 && y1 < N && !used[x1][y1]) {
        x = x1; y = y1; // 更新当前坐标
        used[x][y] = 1; // 标记该位置已被使用
        canvas[x][y] = p(init + step / 2); // 设置颜色值
        break;
      }
      d = (d + 1) % 4; // 改变方向
    }
  }
}

// 视频测试函数
void video_test() {
  unsigned long last = 0; // 上一次更新时间
  unsigned long fps_last = 0; // 上一次显示FPS时间
  int fps = 0; // 帧数计数器

  while (1) {
    unsigned long upt = io_read(AM_TIMER_UPTIME).us / 1000; // 读取当前时间（毫秒）
    if (upt - last > 1000 / FPS) {
      update(); // 更新画布
      redraw(); // 重绘画布
      last = upt; // 更新上一次更新时间
      fps++; // 增加帧数计数器
    }
    if (upt - fps_last > 1000) {
      // 每秒显示一次FPS
      printf("%d: FPS = %d\n", upt, fps);
      fps_last = upt; // 更新上一次显示FPS时间
      fps = 0; // 重置帧数计数器
    }
  }
}
