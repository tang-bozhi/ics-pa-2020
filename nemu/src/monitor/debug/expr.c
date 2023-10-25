#include <isa.h>
#include <stdio.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdlib.h>

enum
{
   TK_NOTYPE = 256,
   '+',
   '-',
   '*',
   '/',
   TK_EQ,
   TK_LPAR,
   TK_RPAR,
   TK_NUM,
};

static struct rule
{
   char* regex;
   int token_type;
} rules[] = {
    {" +", TK_NOTYPE}, // spaces
    {"\\+", '+'},      // plus
    {"-", '-'},        // minus
    {"\\*", '*'},   // multiplication
    {"/", '/'},        // division
    {"==", TK_EQ},     // equal
    {"\\(", TK_LPAR},  // left parenthesis
    {"\\)", TK_RPAR},  // right parenthesis
    {"[0-9]+", TK_NUM} // numbers (at least one digit)
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX] = {}; // 这是一个计算机可以更高效匹配的内部格式，不能在regex之外用这个，应该用rules作为正则模式

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
   int i;
   char error_msg[128];
   int reti;

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

static Token tokens[32] __attribute__((used)) = {}; // 用于存放识别过了的字符串
static int nr_token __attribute__((used)) = 0;      // 识别过了的字符串的数量

static bool make_token(char* e) {
   int position = 0;
   int i;
   regmatch_t pmatch;

   nr_token = 0;

   while (e[position] != '\0') {
      /* Try all rules one by one. */
      for (i = 0; i < NR_REGEX; i++) {
         if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
            char* substr_start = e + position; // 当前循环中被判定字符的指针
            int substr_len = pmatch.rm_eo;     // 当前循环中被判定字符的长度

            Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
               i, rules[i].regex, position, substr_len, substr_len, substr_start);

            position += substr_len; // 挪动e中指针，针对最外层while循环做改变

            // 检查数组tokens是否已满
            if (nr_token >= sizeof(tokens) / sizeof(Token)) {
               printf("token array is full, cannot insert more\n");
               return false;
            }

            if (rules[i].token_type != TK_NOTYPE) { // 抛掉空格

               tokens[nr_token].type = rules[i].token_type; // 设置token类型
               // 将匹配的子字符串复制到token的str字段中
               int length_to_copy = substr_len < sizeof(tokens[nr_token].str) ? substr_len : sizeof(tokens[nr_token].str) - 1;
               strncpy(tokens[nr_token].str, substr_start, length_to_copy);
               tokens[nr_token].str[length_to_copy] = '\0';
            }

            nr_token++;
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

int check_parentheses(int p, int q) {
   if (!(tokens[p].type == TK_LPAR && tokens[q - 1].type == TK_RPAR)) {
      return -1;
   }
   int count = 0;
   for (int i = 0; i < nr_token; i++) {

      if (tokens[i].type == TK_LPAR) {
         count++;
      }
      else if (tokens[i].type == TK_RPAR) {
         count--;
      }
      if (count == 0 && i != p) {
         //(4 + 3)* (2 - 1) false, the leftmost '(' and the rightmost ')' are not matched
         return 0;
      }
      if (count < 0) {
         return -1;
      }
   }
   return (count == 0);
}

int find_main_op(int p, int q) {
   int count = 0;
   int op = -1;
   for (int i = q; i >= p; i++) {
      if (tokens[i].type == TK_LPAR)count++;
      if (tokens[i].type == TK_RPAR)count--;

      if (count == 0) {
         if (tokens[i].type == '+' || tokens[i].type) {
            return i;
         }
         if (tokens[i].type == '*' || tokens[i].type) {
            op = i;
         }
      }
   }
   return op;
}

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
      printf("Unkown type %s.\n", tokens[p].type);
      return -1;
   }
   else if (check_parentheses(p, q) == true) {
      /* The expression is surrounded by a matched pair of parentheses.
       * If that is the case, just throw away the parentheses.
       */
      return eval(p + 1, q - 1);
   }
   else {
      int op = find_main_op(p, q);//principal operator
      for (int i = p; i < q; i++) {
         if (tokens[i].type == '+' || \
            tokens[i].type == '-' || \
            tokens[i].type == '*' || \
            tokens[i].type == '/') {
            op = tokens[p].type;
            break;
         }
      }
      val1 = eval(p, op - 1);
      val2 = eval(op + 1, q);

      switch (op_type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': if (val2 != 0) { return val1 / val2; }
              else {
         printf("Division by zero.\n");
         return -1;
      }
      default: assert(0);
      }
   }
}

word_t expr(char* e, bool* success) {
   if (!make_token(e)) {
      *success = false;
      return 0;
   }
   return eval(0, nr_token - 1);

   return 0;
}
