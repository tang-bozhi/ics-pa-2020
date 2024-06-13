#include <am.h>
#include <nemu.h>

static uint64_t boot_time;

void __am_timer_init() {
   // 获取系统启动时间
   uint32_t low = inl(RTC_ADDR);           // 读取微秒值
   uint32_t high = inl(RTC_ADDR + 4);      // 读取秒值
   boot_time = ((uint64_t)high << 32) | low;  // 组合秒和微秒
}

void __am_timer_uptime(AM_TIMER_UPTIME_T* uptime) {
   uint32_t low = inl(RTC_ADDR);           // 读取微秒值
   uint32_t high = inl(RTC_ADDR + 4);      // 读取秒值
   uint64_t now = ((uint64_t)high << 32) | low;  // 组合秒和微秒
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
