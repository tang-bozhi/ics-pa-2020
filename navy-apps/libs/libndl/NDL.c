#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <assert.h>

#include <fcntl.h>
#include <unistd.h>

static int evtdev = -1;
static int fbdev = -1;
//static int screen_w = 0, screen_h = 0;

/**
 * @brief 获取自系统启动以来的毫秒数
 *
 * @return uint32_t 自系统启动以来的毫秒数
 */
uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000 + tv.tv_usec / 1000; // 自系统启动以来经过的毫秒数
}

/**
 * @brief 轮询检测键盘事件
 *
 * @param buf 缓冲区用于存储事件数据
 * @param len 缓冲区长度
 * @return int 返回1表示有事件，0表示无事件
 */
int NDL_PollEvent(char* buf, int len) {
  int fd = open("/dev/events", 0, 0);
  int ret = read(fd, buf, len);
  assert(close(fd) == 0);
  return ret == 0 ? 0 : 1; // 按键是否被按下
}

// 屏幕大小
static int screen_w = 0, screen_h = 0;
// 画布大小
static int canvas_w = 0, canvas_h = 0;
// 相对于屏幕左上角的画布位置坐标
static int canvas_x = 0, canvas_y = 0;

/**
 * @brief 打开画布并设置其大小
 *
 * 如果指定的宽度和高度为0，则使用屏幕的分辨率。画布的位置居中显示。
 *
 * @param w 画布宽度指针
 * @param h 画布高度指针
 */
void NDL_OpenCanvas(int* w, int* h) {
  printf("in NDL_OpenCanvas:w=%d,h=%d\n", *w, *h);
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // 让 NWM 调整窗口大小并创建帧缓冲区
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
  int buf_size = 1024;
  char* buf = (char*)malloc(buf_size * sizeof(char));
  int fd = open("/proc/dispinfo", 0, 0);
  int ret = read(fd, buf, buf_size);
  printf("%s\n", buf);//printf调试注释
  assert(ret < buf_size); // 为了安全起见
  assert(close(fd) == 0);

  int i = 0;
  int width = 0, height = 0;
  // 解析屏幕的宽度和高度
  assert(strncmp(buf + i, "WIDTH", 5) == 0);
  //这一行将 i 增加 5，以跳过字符串 "WIDTH"。
  i += 5;
  for (; i < buf_size; ++i) {
    if (buf[i] == ':') { i++; break; }
    assert(buf[i] == ' ');
  }
  for (; i < buf_size; ++i) {
    //检查当前字符是否是数字字符。如果是，它跳出循环以开始解析宽度值。
    if (buf[i] >= '0' && buf[i] <= '9') break;
    assert(buf[i] == ' ');
  }
  for (; i < buf_size; ++i) {
    if (buf[i] >= '0' && buf[i] <= '9') {
      //检查当前字符是否是数字字符。如果是，它将当前字符的数字值添加到 width 变量中。
      width = width * 10 + buf[i] - '0';
    }
    else {
      break;
    }
  }
  assert(buf[i++] == '\n');

  assert(strncmp(buf + i, "HEIGHT", 6) == 0);
  i += 6;
  for (; i < buf_size; ++i) {
    if (buf[i] == ':') { i++; break; }
    assert(buf[i] == ' ');
  }
  for (; i < buf_size; ++i) {
    if (buf[i] >= '0' && buf[i] <= '9') break;
    assert(buf[i] == ' ');
  }
  for (; i < buf_size; ++i) {
    if (buf[i] >= '0' && buf[i] <= '9') {
      height = height * 10 + buf[i] - '0';
    }
    else {
      break;
    }
  }

  free(buf);

  screen_w = width;
  screen_h = height;
  printf("%d %d\n", screen_w, screen_h);//printf调试注释
  if (*w == 0 && *h == 0) {
    *w = screen_w;
    *h = screen_h;
  }
  canvas_w = *w;
  canvas_h = *h;
  canvas_x = (screen_w - canvas_w) / 2;
  canvas_y = (screen_h - canvas_h) / 2;
  printf("end of NDL_OpenCanvas:%d %d\n", canvas_w, canvas_h);
}

/**
 * @brief 绘制矩形图像到画布上的指定位置，并同步到屏幕
 *
 * @param pixels 图像像素数据，按行优先方式存储
 * @param x 图像绘制的起始x坐标
 * @param y 图像绘制的起始y坐标
 * @param w 图像宽度
 * @param h 图像高度
 */
void NDL_DrawRect(uint32_t* pixels, int x, int y, int w, int h) {
  int fd = open("/dev/fb", 0, 0);
  for (int i = 0; i < h && y + i < canvas_h; ++i) {
    lseek(fd, ((y + canvas_y + i) * screen_w + (x + canvas_x)) * 4, SEEK_SET);
    write(fd, pixels + i * w, 4 * (w < canvas_w - x ? w : canvas_w - x));
  }
  assert(close(fd) == 0);
}

/**
 * @brief 初始化音频系统
 *
 * @param freq 采样频率
 * @param channels 声道数
 * @param samples 样本数
 */
void NDL_OpenAudio(int freq, int channels, int samples) {
}

/**
 * @brief 关闭音频系统
 */
void NDL_CloseAudio() {
}

/**
 * @brief 播放音频数据
 *
 * @param buf 音频数据缓冲区
 * @param len 音频数据长度
 * @return int 返回写入的字节数
 */
int NDL_PlayAudio(void* buf, int len) {
  return 0;
}

/**
 * @brief 查询音频播放状态
 *
 * @return int 返回剩余的可播放字节数
 */
int NDL_QueryAudio() {
  return 0;
}

/**
 * @brief 初始化NDL库
 *
 * @param flags 初始化标志
 * @return int 返回0表示成功
 */
int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

/**
 * @brief 退出NDL库
 */
void NDL_Quit() {
}
