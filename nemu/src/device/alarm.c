#include <common.h>
#include <sys/time.h>
#include <signal.h>

#define TIMER_HZ 60  // 定义定时器的频率为每秒60次
#define MAX_HANDLER 8  // 最大处理函数数量

// 定义定时器处理函数的类型
typedef void (*alarm_handler_t) ();
static alarm_handler_t handler[MAX_HANDLER] = {};  // 存储定时器处理函数的数组
static int idx = 0;  // 当前已注册的处理函数数量
static uint32_t jiffy = 0;  // 系统启动以来的滴答数

// 添加一个定时器处理函数
void add_alarm_handle(void* h) {
  assert(idx < MAX_HANDLER);  // 确保没有超出处理函数的最大数量
  handler[idx++] = h;  // 将新的处理函数添加到数组中，并增加索引
}

// 获取系统的运行时间（以秒为单位）
uint32_t uptime() { return jiffy / TIMER_HZ; }

// 定时器信号处理函数
static void alarm_sig_handler(int signum) {
  int i;
  for (i = 0; i < idx; i++) {
    handler[i]();  // 调用所有已注册的处理函数
  }

  jiffy++;  // 每次信号处理后增加滴答数
}

// 初始化定时器
void init_alarm() {
  struct sigaction s;
  memset(&s, 0, sizeof(s));  // 初始化sigaction结构体
  s.sa_handler = alarm_sig_handler;  // 设置信号处理函数
  int ret = sigaction(SIGVTALRM, &s, NULL);  // 注册虚拟定时器信号处理函数
  Assert(ret == 0, "Can not set signal handler");  // 断言注册成功

  struct itimerval it = {};  // 初始化时间间隔结构体
  it.it_value.tv_sec = 0;  // 设置定时器的初次触发时间（秒）
  it.it_value.tv_usec = 1000000 / TIMER_HZ;  // 设置定时器的初次触发时间（微秒）
  it.it_interval = it.it_value;  // 设置定时器的重复触发时间间隔
  ret = setitimer(ITIMER_VIRTUAL, &it, NULL);  // 启动虚拟定时器
  Assert(ret == 0, "Can not set timer");  // 断言定时器启动成功
}
