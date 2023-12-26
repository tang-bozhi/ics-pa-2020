//通过gen-expr.c测试
#include <isa.h>
#include <stdio.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdlib.h>

enum {
   TK_NOTYPE = 256,// 空格 256
   TK_PLUS = '+',        // 加号 43
   TK_MINUS = '-',       // 减号 45
   TK_STAR = '*',        // 乘号 42
   TK_SLASH = '/',       // 除号 47
   TK_NEG = 257,         // 负数 与TK_MINUS减法区别 257
   TK_EQ,          // 等号 258
   TK_LPAR,        // 左括号 259
   TK_RPAR,        // 右括号 260
   TK_NUM,          // 数字 261
};


static struct rule
{
   char* regex;
   int token_type;
} rules[] = {
    {" +", TK_NOTYPE}, // spaces
    {"\\+", TK_PLUS},          // plus
    {"-", TK_MINUS},        // minus
    {"-", TK_NEG},     // negtive sign
    {"\\*", TK_STAR},      // multiplication
    {"/", TK_SLASH},        // division
    {"==", TK_EQ},     // equal
    {"\\(", TK_LPAR},  // left parenthesis
    {"\\)", TK_RPAR},  // right parenthesis
    {"[0-9]+", TK_NUM} // numbers (at least one digit)
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX] = {}; // regex.h 这是一个计算机可以更高效匹配的内部格式，不能在regex之外用这个，应该用rules作为正则模式

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex_add_op() {
   int i;
   char error_msg[128];
   int reti;//return value of regex compile

   for (i = 0; i < NR_REGEX; i++) {
      reti = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
      if (reti != 0) {
         regerror(reti, &re[i], error_msg, 128);
         panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
      }
   }
}

typedef struct token
{
   int type;
   char str[32];
} Token;

static Token tokens[1000] __attribute__((used)) = {}; // 用于存放识别过了的字符串
static int nr_token __attribute__((used)) = 0;      // 识别过了的字符串的数量

// Converts an expression string into tokens based on the defined regex rules.
// @param e: String expression to tokenize
// @return: Returns true if tokenization is successful, false otherwise
static bool make_token(char* e) {
   int position = 0;//字符串当前处理位置
   int i;
   regmatch_t pmatch;//匹配结果

   nr_token = 0;
   //e是输入的被判定字符串
   while (e[position] != '\0') {
      /* Try all rules one by one. */
      for (i = 0; i < NR_REGEX; i++) {
         if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
            char* substr_start = e + position; // 当前循环中被判定字符的指针
            int substr_len = pmatch.rm_eo;     // 当前循环中被判定字符的长度

            int if_true_1 = 0;//检查if是否执行了内部代码
            int if_true_2 = 0;

            Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
               i, rules[i].regex, position, substr_len, substr_len, substr_start);

            // 挪动e中指针，针对最外层while循环做改变
            position += substr_len;

            // 检查数组tokens是否已满
            if (nr_token >= sizeof(tokens) / sizeof(Token)) {
               printf("token array is full, cannot insert more\n");
               return false;
            }

            //判断是减号还是负号
            if (rules[i].token_type == TK_MINUS) {//对:  文件开头-,(-,=-  三种情况做判定
               if (nr_token == 0 || tokens[nr_token - 1].type == TK_LPAR || tokens[nr_token - 1].type == TK_EQ) {
                  tokens[nr_token].type = TK_NEG;
                  if_true_1++;
               }
            }

            if (rules[i].token_type != TK_NOTYPE) {// 抛掉空格
               tokens[nr_token].type = rules[i].token_type; // 设置token类型
               // 下方三行  将匹配的子字符串复制到token的str字段中

               int length_to_copy = substr_len < sizeof(tokens[nr_token].str) ? substr_len : sizeof(tokens[nr_token].str) - 1;
               strncpy(tokens[nr_token].str, substr_start, length_to_copy);
               tokens[nr_token].str[length_to_copy] = '\0';
               if_true_2++;
            }
            if (if_true_1 || if_true_2) {
               nr_token++;//如果识别到了token type,则nr_token++
            }
            break;
         }
      }

      if (i == NR_REGEX) {
         printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
         return false;
      }
   }
   return true;
}

//检查括号是否有且正确
int check_parentheses(int p, int q) {
   int count = 0;
   for (int i = p; i <= q; i++) {
      if (tokens[i].type == TK_LPAR) {
         count++;
      }
      else if (tokens[i].type == TK_RPAR) {
         count--;
      }
      if (count == 0 && i != q && i != p) {
         //(4 + 3)* (2 - 1) false, the leftmost '(' and the rightmost ')' are not matched
         return i + 1;
      }
      if (count < 0) {
         return -1;
      }
   }
   return (count == 0);
}

//find main operator
int find_main_op(int p, int q) {
   int count = 0;
   int main_op = -1;
   int last_found_mul_div = -1;//用来将*或/限制在右端第一个
   printf("find_main_op called with p = %d, q = %d\n", p, q);
   for (int i = q; i != p; i--) {
      if (tokens[i].type == TK_NOTYPE) {
         continue;
      }

      if (tokens[i].type == TK_LPAR) {
         count++;
      }
      if (tokens[i].type == TK_RPAR) {
         count--;
      }

      if (count == 0) {
         if (tokens[i].type == TK_PLUS || tokens[i].type == TK_MINUS) {
            return i;
         }
         if ((tokens[i].type == TK_STAR || tokens[i].type == TK_SLASH) && last_found_mul_div == -1) {
            last_found_mul_div = i;
            main_op = i;
         }
      }
   }
   if (last_found_mul_div != -1) {
      return last_found_mul_div;
   }
   return main_op;
}

//evaluate
//这个函数是通过教案指导的分治法也就是那嵌套的几行exp<>写出来的
int eval(int p, int q) {
   if (p > q) {
      /* Bad expression */
      printf("Bad expression from %d to %d.\n", p, q);
      return -1;
   }
   else if (p == q) {
      /* Single token.
       * For now this token should be a number.
       * Return the value of the number.
       */
      if (tokens[p].type == TK_NUM) {
         return (atoi(tokens[p].str));
      }
      printf("Unkown type %d.\n", tokens[p].type);
      return -1;
   }
   else if (check_parentheses(p, q) == 1) {
      /* The expression is surrounded by a matched pair of parentheses.
       * If that is the case, just throw away the parentheses.
       */
      int result = eval(p + 1, q - 1);
      return result;
   }
   else {
      int op = find_main_op(p, q);//principal operator
      if (op == -1) {
         int check_par_result = check_parentheses(p, q);
         if (check_par_result == -1) {
            printf("No main operator found from %d to %d.\n", p, q);
            return -1;
         }
         else if (check_par_result > 1) {
            op = check_par_result;
         }
      }
      int val1 = 0, val2 = 0;
      if (op != p) { // 确保左侧表达式存在
         val1 = eval(p, op - 1);
      }
      if (op != q) { // 确保右侧表达式存在
         val2 = eval(op + 1, q);
      }

      switch (tokens[op].type) {
      case TK_PLUS:  return val1 + val2;
      case TK_MINUS: return val1 - val2;
      case TK_NEG:   return -eval(p + 1, q);
      case TK_STAR:  return val1 * val2;
      case TK_SLASH:
         if (val2 != 0) { return val1 / val2; }
         else {
            printf("Division by zero.\n");
            return -1;
         }
      default: assert(0);
      }
   }
}

//express
word_t expr(char* e, bool* success) {
   if (!make_token(e)) {
      *success = false;
      return 0;
   }

   *success = true; // Set success to true as make_token succeeded
   return eval(0, nr_token - 1);
}

