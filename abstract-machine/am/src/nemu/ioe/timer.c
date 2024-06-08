#include <am.h>
#include <nemu.h>

static uint64_t boot_time;

void __am_timer_config(AM_TIMER_CONFIG_T* config) {
   config->present = config->has_rtc = true;  // 表示时钟设备存在
}

void __am_timer_init() {
   // 获取系统启动时间
   boot_time = inl(RTC_ADDR);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T* uptime) {
   uint64_t now = inl(RTC_ADDR);
   uptime->us = now - boot_time;
}

// void __am_timer_rtc(AM_TIMER_RTC_T* rtc) {
//    // 如果实现RTC功能，需要从RTC硬件读取实时时间
//    // 目前返回固定值，符合当前教学需求
//    rtc->second = 0;
//    rtc->minute = 0;
//    rtc->hour = 0;
//    rtc->day = 0;
//    rtc->month = 0;
//    rtc->year = 1900;
//}

void __am_timer_rtc(AM_TIMER_RTC_T* rtc) {
   static int tick = 0; // 模拟时钟滴答
   rtc->second = tick % 60;
   rtc->minute = (tick / 60) % 60;
   rtc->hour = (tick / 3600) % 24;
   rtc->day = 1;
   rtc->month = 1;
   rtc->year = 2023;
   tick++;
}
