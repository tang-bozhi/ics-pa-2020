#include <amtest.h>

void rtc_test() {
  //AM_TIMER_RTC_T rtc;
  int sec = 1;
  while (1) {
    while (io_read(AM_TIMER_UPTIME).us / 1000000 < sec);
    long int us = io_read(AM_TIMER_UPTIME).us;
    //rtc = io_read(AM_TIMER_RTC);
    //printf("%d-%d-%d %02d:%02d:%02d GMT (", rtc.year, rtc.month, rtc.day, rtc.hour, rtc.minute, rtc.second);
    if (sec == 1) {
      printf("%lld us).\n", us);
      //printf("%d second).\n", sec);
    }
    else {
      printf("%lld us).\n", us);
      //printf("%d seconds).\n", sec);
    }
    sec++;
  }
}
