#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char* code_format =
"#include <stdio.h>\n"
"int main() {\n "
"  unsigned result = %s;\n "
"  printf(\"%%u\", result);\n "
"  return 0;\n "
"}";

//generate a number less than n
int choose(int n) {
   return rand() % n;
}

void insert_rand_space() {
   if (choose(2) && strlen(buf) + 2 < sizeof(buf)) {
      strcat(buf, " ");
   }
}

void gen_num() {
   if (strlen(buf) + 100 < sizeof(buf)) {
      //buf[strlen(buf)] = '0' + choose(10);//这个也对，但感觉最好不要这么写，容易错
      sprintf(buf + strlen(buf), "%d", choose(10));//这里buf + strlen(buf)指针被改变前存放\0
   }
}

void gen_rand_op() {
   char ops[] = "+-*/";
   if (strlen(buf) + 100 < sizeof(buf)) {
      buf[strlen(buf)] = ops[choose(4)];
   }
}

void gen_rand_exp() {
   switch (choose(3)) {
   case 0:
      gen_num();
      break;
   case 1:
      strcat(buf, "(");
      gen_rand_exp();
      //gen_rand_op();
      //gen_rand_exp();
      strcat(buf, ")");
      break;
   default:
      gen_rand_exp();
      gen_rand_op();
      // if we are generating a division operation, make sure the divisor is not zero.
      if (buf[strlen(buf) - 1] == '/') {
         int divisor;
         do {
            divisor = choose(10);
         } while (divisor == 0);
         sprintf(buf + strlen(buf), "%d", divisor);
      }
      else {
         gen_rand_exp();
      }
      break;
   }
}

int main(int argc, char* argv[]) {
   int seed = time(0);
   srand(seed);
   int loop = 2;
   if (argc > 1) {
      sscanf(argv[1], "%d", &loop);
   }
   int i;
   for (i = 0; i < loop; i++) {
      gen_rand_exp();

      sprintf(code_buf, code_format, buf);

      FILE* fp = fopen("/tmp/.code.c", "w");
      assert(fp != NULL);
      fputs(code_buf, fp);
      fclose(fp);

      int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
      if (ret != 0) continue;

      fp = popen("/tmp/.expr", "r");
      assert(fp != NULL);

      int result;
      fscanf(fp, "%d", &result);
      pclose(fp);

      printf("%u %s\n", result, buf);
   }
   return 0;
}
