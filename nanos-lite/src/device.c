#include <common.h>

// 如果定义了 MULTIPROGRAM 且未定义 TIME_SHARING，
// 则调用 yield() 允许在任务之间切换。否则不执行任何操作。
#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

// 宏定义，用于将按键名称映射为其字符串表示。
#define NAME(key) \
  [AM_KEY_##key] = #key,

// 数组存储每个按键的名称，用于将按键代码转换为可读的字符串。
static const char* keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

// 向串口写入数据。
// 参数：
// - buf: 指向要写入的数据的指针。
// - offset: 起始位置（在此函数中未使用）。
// - len: 要写入的字节数。
// 返回写入的字节数。
size_t serial_write(const void* buf, size_t offset, size_t len) {
  for (size_t i = 0; i < len; ++i) putch(*((char*)buf + i));
  return len;
}

// 读取键盘事件并将事件信息写入提供的缓冲区。
// 参数：
// - buf: 存储事件信息的缓冲区。
// - offset: 起始位置（在此函数中未使用）。
// - len: 缓冲区的大小。
// 返回写入缓冲区的字节数，如果没有事件发生则返回0。
size_t events_read(void* buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if (ev.keycode == AM_KEY_NONE) {
    *(char*)buf = '\0';
    return 0;
  }
  int ret = snprintf(buf, len, "%s %s\n", ev.keydown ? "kd" : "ku", keyname[ev.keycode]);
  printf("%s\n", buf);
  return ret;
}

// 读取显示信息并将其写入提供的缓冲区。
// 参数：
// - buf: 存储显示信息的缓冲区。
// - offset: 起始位置（在此函数中未使用）。
// - len: 缓冲区的大小。
// 返回写入缓冲区的字节数。
size_t dispinfo_read(void* buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T t = io_read(AM_GPU_CONFIG);
  return snprintf(buf, len, "WIDTH:%d\nHEIGHT:%d\n", t.width, t.height);
}

// 向帧缓冲区写入像素数据。
// 参数：
// - buf: 指向要写入的像素数据的指针。
// - offset: 帧缓冲区的起始位置（字节偏移量）。
// - len: 要写入的字节数。
// 返回写入的像素数。
size_t fb_write(const void* buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T ev = io_read(AM_GPU_CONFIG);
  int width = ev.width;

  offset /= 4; // 将字节偏移量转换为像素偏移量（假设每像素4字节）。
  len /= 4;    // 将字节长度转换为像素长度。

  int y = offset / width; // 根据像素偏移量计算行号。
  int x = offset - y * width; // 根据行号和总宽度计算列号。

  // 在指定位置将像素数据写入帧缓冲区。
  io_write(AM_GPU_FBDRAW, x, y, (void*)buf, len, 1, true);

  return len;
}

// 初始化I/O设备。
// 该函数设置设备通信所需的配置。
void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
