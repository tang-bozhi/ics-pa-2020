#include "trap.h"

void test_printf() {
   printf("Test printf: %d, %f, %s, %ld, %lld\n", 42, 3.14, "hello", 123456789012345L, 1234567890123456789LL);
}

void test_sprintf() {
   char buffer[1024];
   sprintf(buffer, "Test sprintf: %d, %f, %s, %ld, %lld", 42, 3.14, "hello", 123456789012345L, 1234567890123456789LL);
   printf("Output of sprintf: %s\n", buffer);
}

void test_snprintf() {
   char buffer[50];
   snprintf(buffer, sizeof(buffer), "Test snprintf: %d, %f, %s, %ld, %lld", 42, 3.14, "hello world this is a test", 123456789012345L, 1234567890123456789LL);
   printf("Output of snprintf (50 chars max): %s\n", buffer);
}

void test_snprintf_overflow() {
   char buffer[25];
   int result = snprintf(buffer, sizeof(buffer), "This is a long long test %lld", 1234567890123456789LL);
   printf("Output of snprintf with overflow (25 chars max): %s\n", buffer);
   printf("Reported length: %d (should be length if no limit was set)\n", result);
}

int main() {
   test_printf();
   test_sprintf();
   test_snprintf();
   test_snprintf_overflow();
   return 0;
}
