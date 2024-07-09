#include <am.h>
#include <klib.h>
#include <klib-macros.h>

// 定义常量
#define FPS            30  // 每秒帧数
#define CPS             5  // 每秒字符出现数
#define CHAR_W          8  // 字符宽度
#define CHAR_H         16  // 字符高度
#define NCHAR         128  // 最大字符数
#define COL_WHITE    0xeeeeee  // 白色
#define COL_RED      0xff0033  // 红色
#define COL_GREEN    0x00cc33  // 绿色
#define COL_PURPLE   0x2a0a29  // 紫色

// 定义颜色枚举
enum { WHITE = 0, RED, GREEN, PURPLE };

// 字符结构体定义
struct character {
  char ch;  // 字符
  int x, y;  // 字符位置
  int v;     // 字符速度
  int t;     // 字符消失时间
} chars[NCHAR];  // 字符数组

// 全局变量定义
int screen_w, screen_h, hit, miss, wrong;
uint32_t texture[3][26][CHAR_W * CHAR_H], blank[CHAR_W * CHAR_H];

// 获取最小值函数
int min(int a, int b) {
  return (a < b) ? a : b;
}

// 生成随机数函数
int randint(int l, int r) {
  return l + (rand() & 0x7fffffff) % (r - l + 1);
}

// 生成新字符
void new_char() {
  for (int i = 0; i < LENGTH(chars); i++) {
    struct character* c = &chars[i];
    if (!c->ch) {
      c->ch = 'A' + randint(0, 25);  // 随机生成字符
      c->x = randint(0, screen_w - CHAR_W);  // 随机位置
      c->y = 0;
      c->v = (screen_h - CHAR_H + 1) / randint(FPS * 3 / 2, FPS * 2);  // 随机速度
      c->t = 0;
      return;
    }
  }
}

// 更新游戏逻辑
void game_logic_update(int frame) {
  if (frame % (FPS / CPS) == 0) new_char();  // 每隔一定时间生成新字符
  for (int i = 0; i < LENGTH(chars); i++) {
    struct character* c = &chars[i];
    if (c->ch) {
      if (c->t > 0) {
        if (--c->t == 0) {
          c->ch = '\0';
        }
      }
      else {
        c->y += c->v;
        if (c->y < 0) {
          c->ch = '\0';
        }
        if (c->y + CHAR_H >= screen_h) {
          miss++;
          c->v = 0;
          c->y = screen_h - CHAR_H;
          c->t = FPS;
        }
      }
    }
  }
}

// 渲染函数
void render() {
  static int x[NCHAR], y[NCHAR], n = 0;

  // 清除上一帧的字符
  for (int i = 0; i < n; i++) {
    io_write(AM_GPU_FBDRAW, x[i], y[i], blank, CHAR_W, CHAR_H, false);
  }

  n = 0;
  // 绘制当前帧的字符
  for (int i = 0; i < LENGTH(chars); i++) {
    struct character* c = &chars[i];
    if (c->ch) {
      x[n] = c->x; y[n] = c->y; n++;
      int col = (c->v > 0) ? WHITE : (c->v < 0 ? GREEN : RED);  // 颜色根据字符状态变化
      io_write(AM_GPU_FBDRAW, c->x, c->y, texture[col][c->ch - 'A'], CHAR_W, CHAR_H, false);
    }
  }
  io_write(AM_GPU_FBDRAW, 0, 0, NULL, 0, 0, true);
  for (int i = 0; i < 40; i++) putch('\b');
  printf("Hit: %d; Miss: %d; Wrong: %d", hit, miss, wrong);
}

// 检查击中字符
void check_hit(char ch) {
  int m = -1;
  for (int i = 0; i < LENGTH(chars); i++) {
    struct character* c = &chars[i];
    if (ch == c->ch && c->v > 0 && (m < 0 || c->y > chars[m].y)) {
      m = i;
    }
  }
  if (m == -1) {
    wrong++;
  }
  else {
    hit++;
    chars[m].v = -(screen_h - CHAR_H + 1) / (FPS);  // 反向飞出屏幕
  }
}

// 视频初始化
// 初始化视频系统，设置屏幕尺寸
void video_init() {
  // 从硬件配置中读取屏幕宽度和高度
  screen_w = io_read(AM_GPU_CONFIG).width;
  screen_h = io_read(AM_GPU_CONFIG).height;

  // 外部声明的字体数据数组
  extern char font[];
  // 初始化`blank`数组，这将用于清除屏幕上的字符
  for (int i = 0; i < CHAR_W * CHAR_H; i++)
    blank[i] = COL_PURPLE;

  // 使用`blank`数组中的颜色填充整个屏幕
  // 这里将屏幕划分为若干个CHAR_W x CHAR_H的块，然后用紫色填充每一个块
  for (int x = 0; x < screen_w; x += CHAR_W)
    for (int y = 0; y < screen_h; y += CHAR_H) {
      io_write(AM_GPU_FBDRAW, x, y, blank, min(CHAR_W, screen_w - x), min(CHAR_H, screen_h - y), false);
    }

  // 初始化字符纹理数据，用于在屏幕上绘制字符
  // 这部分循环遍历26个英文大写字母
  for (int ch = 0; ch < 26; ch++) {
    char* c = &font[CHAR_H * ch];  // 指向字体数据中每个字符的起始位置
    // 对每个字符的每一行进行处理
    for (int i = 0, y = 0; y < CHAR_H; y++)
      for (int x = 0; x < CHAR_W; x++, i++) {
        int t = (c[y] >> (CHAR_W - x - 1)) & 1;  // 提取字符数据中的位信息，生成字符的图像
        texture[WHITE][ch][i] = t ? COL_WHITE : COL_PURPLE;  // 根据位的值，设置纹理颜色为白色或紫色背景
        texture[GREEN][ch][i] = t ? COL_GREEN : COL_PURPLE;  // 设置纹理颜色为绿色或紫色背景
        texture[RED][ch][i] = t ? COL_RED : COL_PURPLE;      // 设置纹理颜色为红色或紫色背景
      }
  }
}

// 键盘字符对应表
char lut[256] = {
  [AM_KEY_A] = 'A',[AM_KEY_B] = 'B',[AM_KEY_C] = 'C',[AM_KEY_D] = 'D',
  [AM_KEY_E] = 'E',[AM_KEY_F] = 'F',[AM_KEY_G] = 'G',[AM_KEY_H] = 'H',
  [AM_KEY_I] = 'I',[AM_KEY_J] = 'J',[AM_KEY_K] = 'K',[AM_KEY_L] = 'L',
  [AM_KEY_M] = 'M',[AM_KEY_N] = 'N',[AM_KEY_O] = 'O',[AM_KEY_P] = 'P',
  [AM_KEY_Q] = 'Q',[AM_KEY_R] = 'R',[AM_KEY_S] = 'S',[AM_KEY_T] = 'T',
  [AM_KEY_U] = 'U',[AM_KEY_V] = 'V',[AM_KEY_W] = 'W',[AM_KEY_X] = 'X',
  [AM_KEY_Y] = 'Y',[AM_KEY_Z] = 'Z',
};

// 主函数
int main() {
  ioe_init();
  video_init();

  // 检查定时器和键盘输入
  panic_on(!io_read(AM_TIMER_CONFIG).present, "requires timer");
  panic_on(!io_read(AM_INPUT_CONFIG).present, "requires keyboard");

  printf("Type 'ESC' to exit\n");

  int current = 0, rendered = 0;
  while (1) {
    int frames = io_read(AM_TIMER_UPTIME).us / (1000000 / FPS);

    // 更新游戏逻辑
    for (; current < frames; current++) {
      game_logic_update(current);
    }

    // 处理键盘输入
    while (1) {
      AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
      if (ev.keycode == AM_KEY_NONE) break;
      if (ev.keydown && ev.keycode == AM_KEY_ESCAPE) halt(0);
      if (ev.keydown && lut[ev.keycode]) {
        check_hit(lut[ev.keycode]);
      }
    };

    // 渲染游戏
    if (current > rendered) {
      render();
      rendered = current;
    }
  }
}
