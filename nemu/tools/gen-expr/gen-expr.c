//pa1-5表达式求值-如何测试你的代码-暂时认为完成
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <regex.h>

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
   insert_rand_space();
   if (strlen(buf) + 100 < sizeof(buf)) {
      //buf[strlen(buf)] = '0' + choose(10);//这个也对，但感觉最好不要这么写，容易错
      sprintf(buf + strlen(buf), "%d", choose(10));//这里buf + strlen(buf)指针被改变前存放\0
   }
   insert_rand_space();
}

void gen_rand_op() {
   insert_rand_space();
   char ops[] = "+-*/";
   if (strlen(buf) + 100 < sizeof(buf)) {
      buf[strlen(buf)] = ops[choose(4)];
   }
   insert_rand_space();
}

//下方使用正则表达式过滤"[0-9]+\\("情况
static regex_t regex;
static int ret;
static regmatch_t matchs[2];//保存匹配结果 

void init_regex() {
   ret = regcomp(&regex, "\\d+\\(|\\)\\d+", REG_EXTENDED);
   assert(ret == 0);  // 确保正则表达式编译成功
}

void adding_rand_op() {
   int offset = 0;
   char ops[] = "+-*/";
   init_regex();
   while (!regexec(&regex, buf + offset, 1, matchs, 0)) {
      memmove(buf + offset + matchs[0].rm_eo, buf + offset + matchs[0].rm_eo, sizeof(buf) - offset - matchs[0].rm_eo);
      buf[offset + matchs[0].rm_eo] = ops[choose(4)];
      offset += matchs[0].rm_eo + 1;
   }
   insert_rand_space();
}

void gen_rand_exp() {
   insert_rand_space();
   static int deepth = 0;
   static int max_deep = 100;
   if (deepth >= max_deep) {
      gen_num();
      return;
   }
   switch (choose(3)) {
   case 0:
      deepth++;
      gen_num();
      break;
   case 1:
      deepth++;
      strcat(buf, "(");
      gen_rand_exp();
      gen_rand_op();
      gen_rand_exp();
      strcat(buf, ")");
      break;
   default:
      deepth++;
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
   int loop = 1;
   if (argc > 1) {
      sscanf(argv[1], "%d", &loop);
   }
   int i;
   for (i = 0; i < loop; i++) {
      memset(buf, 0, sizeof(buf));
      gen_rand_exp();

      adding_rand_op();

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
   regfree(&regex);
   return 0;
}
