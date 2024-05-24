#include <am.h>
#include <nemu.h>

static uint64_t boot_time;

void __am_timer_init() {
   // 获取系统启动时间
   boot_time = inl(RTC_ADDR);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T* uptime) {
   uint64_t now = inl(RTC_ADDR);
   // 假设 RTC_ADDR 返回从系统启动至今的微秒数
   uptime->us = now - boot_time;
}

void __am_timer_rtc(AM_TIMER_RTC_T* rtc) {
   // 如果实现RTC功能，需要从RTC硬件读取实时时间
   // 目前返回固定值，符合当前教学需求
   rtc->second = 0;
   rtc->minute = 0;
   rtc->hour = 0;
   rtc->day = 0;
   rtc->month = 0;
   rtc->year = 1900;
}
