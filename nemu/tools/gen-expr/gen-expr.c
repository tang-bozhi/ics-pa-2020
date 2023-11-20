//pa1-5表达式求值-如何测试你的代码-暂时认为完成
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <regex.h>
#include <unistd.h>

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

// void insert_rand_space() {
//    if (choose(2) && strlen(buf) + 2 < sizeof(buf)) {
//       strcat(buf, " ");
//    }
// }

void gen_num() {//直接copy的
   // 确保 buf 中有足够的空间
   if (strlen(buf) + 100 < sizeof(buf)) {
      char num_str[12];  // 用于存储生成的数字字符串

      // 生成第一个非零数字
      int first_digit = choose(9) + 1;  // 这将生成 1 到 9 之间的数字
      sprintf(num_str, "%d", first_digit);

      // 生成随机长度的数字（例如，可以生成 1 到 2 位的数字）
      int num_length = choose(2) + 1;
      for (int i = 1; i < num_length; ++i) {
         int digit = choose(10);  // 生成 0 到 9 之间的数字
         sprintf(num_str + strlen(num_str), "%d", digit);
      }

      // 将生成的数字字符串添加到 buf
      strcat(buf, num_str);
   }
}


void gen_rand_op() {
   char ops[] = "+-*/";
   if (strlen(buf) + 100 < sizeof(buf)) {
      buf[strlen(buf)] = ops[choose(4)];
   }
}

//下方使用正则表达式过滤"[0-9]+\\("情况
static regex_t regex;
static int ret;
static regmatch_t matchs[2];//保存匹配结果 

void init_regex() {
   ret = regcomp(&regex, "([0-9]+ *)+\\(|\\)([0-9]+ *)+", REG_EXTENDED);
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
}

static int deepth = 0;//如果放到函数内部,这个一定要静态
static int max_deep = 200;
static int min_deep = 20;

void gen_rand_expr();//提前声明

void conti_gen_if_shallow() {
   if (deepth <= min_deep) {
      gen_rand_expr();
      return;
   }
}
#define break_if_too_deep \
if (deepth >= max_deep) {\
   return;\
}

void gen_rand_expr() {
   switch (choose(3)) {
   case 0:
      break_if_too_deep;
      deepth++;
      gen_num();
      break;
   case 1:
      break_if_too_deep;
      deepth++;
      // 确保左括号前有运算符
      if (strlen(buf) > 0 && strchr("+-*/", buf[strlen(buf) - 1]) == NULL && buf[strlen(buf) - 1] != '(') {//buf[strlen(buf) - 1]是对最末尾字符做检测,buf[strlen(buf)]则是'\0' 
         gen_rand_op();
      }
      strcat(buf, "(");
      gen_rand_expr();
      strcat(buf, ")");
      break;
   default:
      break_if_too_deep;
      deepth++;
      gen_rand_expr();
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
         gen_rand_expr();
      }
      break;
   }
   conti_gen_if_shallow();
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

      gen_rand_expr();

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
   //sleep(1);
   return 0;
}
