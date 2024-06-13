#include <common.h>

#ifdef HAS_IOE  // 如果定义了HAS_IOE，表示支持输入输出设备

#include <device/alarm.h>
#include <SDL2/SDL.h>
#include <monitor/monitor.h>

// 声明初始化各种设备的函数
void init_alarm();
void init_serial();
void init_timer();
void init_vga();
void init_i8042();
void init_audio();

// 声明用于发送按键信息的函数
void send_key(uint8_t, bool);
// 声明用于更新VGA显示的函数
void vga_update_screen();

// 设备更新标志，用于控制设备更新操作的执行
static int device_update_flag = false;

// 设置设备更新标志的函数
static void set_device_update_flag() {
  device_update_flag = true;  // 将设备更新标志置为真，表示需要进行设备更新
}

// 设备更新函数
void device_update() {
  if (!device_update_flag) {  // 如果设备更新标志未被设置，则直接返回
    return;
  }
  device_update_flag = false;  // 重置设备更新标志
  vga_update_screen();  // 更新VGA屏幕显示

  // 处理SDL事件
  SDL_Event event;
  while (SDL_PollEvent(&event)) {  // 循环处理所有SDL事件
    switch (event.type) {
    case SDL_QUIT:  // 如果是退出事件
      nemu_state.state = NEMU_QUIT;  // 设置模拟器状态为退出
      break;
    case SDL_KEYDOWN:  // 按键按下事件
    case SDL_KEYUP:  // 按键释放事件
    {
      uint8_t k = event.key.keysym.scancode;  // 获取按键的扫描码
      bool is_keydown = (event.key.type == SDL_KEYDOWN);  // 判断是按下还是释放
      send_key(k, is_keydown);  // 发送按键信息
      break;
    }
    default: break;  // 其他事件不处理
    }
  }
}

// 清除SDL事件队列的函数
void sdl_clear_event_queue() {
  SDL_Event event;
  while (SDL_PollEvent(&event));  // 循环获取事件直到事件队列为空
}

// 初始化设备的函数
void init_device() {
  // 初始化各种设备
  init_serial();
  init_timer();
  init_vga();
  init_i8042();
  init_audio();

  // 注册设备更新标志设置函数到定时器处理函数列表
  add_alarm_handle(set_device_update_flag);
  // 初始化定时器
  init_alarm();
}
#else

// 如果未定义HAS_IOE，定义一个空的init_device函数
void init_device() {
}

#endif  /* HAS_IOE */
