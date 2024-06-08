#include <amtest.h>

void rtc_test() {
  AM_TIMER_RTC_T rtc;
  int sec = 1;
  uint64_t last_us = 0; // Variable to store the last microsecond timestamp

  while (1) {
    while (1) {
      uint64_t current_us = io_read(AM_TIMER_UPTIME).us;
      if (current_us / 1000000 >= sec) {
        break;
      }
      if (current_us == last_us) {
        // If the time has not incremented, print a warning
        printf("WARNING: Time has not incremented, stuck at %lu microseconds.\n", current_us);
      }
      else if (current_us < last_us) {
        // If the time has decreased, print a warning
        printf("WARNING: Time has decreased from %lu to %lu microseconds.\n", last_us, current_us);
      }
      last_us = current_us;
    }
    rtc = io_read(AM_TIMER_RTC);
    printf("%d-%d-%d %02d:%02d:%02d GMT (", rtc.year, rtc.month, rtc.day, rtc.hour, rtc.minute, rtc.second);
    if (sec == 1) {
      printf("%d second).\n", sec);
    }
    else {
      printf("%d seconds).\n", sec);
    }
    sec++;
    last_us = io_read(AM_TIMER_UPTIME).us; // Update last_us to the latest read time
  }
}

