#include <isa.h>
#include "expr.h"
#include "watchpoint.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>

extern WP* get_head_wp();

void isa_reg_display(void);
void cpu_exec(uint64_t);
int is_batch_mode();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
   static char* line_read = NULL;

   if (line_read) {
      free(line_read);
      line_read = NULL;
   }

   line_read = readline("(nemu) ");

   if (line_read && *line_read) {
      add_history(line_read);
   }

   return line_read;
}
//continue
static int cmd_c(char* args);
//quit
static int cmd_q(char* args);
//help
static int cmd_help(char* args);
//step implementation
static int cmd_si(char* args);
//Infomation -r -w
static int cmd_info(char* args);
//Scanning memory
static int cmd_x(char* args);
//Print $EXPR 
static int cmd_p(char* args);
//Setting up watchpoints
static int cmd_w(char* args);
//Deleting Watchpoints
static int cmd_d(char* args);

//别名数组
static char* help_aliases[] = { "h", "info", "information", NULL };
static char* continue_aliases[] = { "cont", "run", "go", NULL };
static char* quit_aliases[] = { "q", "exit", "close", NULL };
static char* step_aliases[] = { "s", "stepi", "next", NULL };
static char* info_aliases[] = { "i", "status", "stat", NULL };
static char* scan_aliases[] = { "scan", "examine", "dump", NULL };
static char* print_aliases[] = { "pr", "evaluate", "eval", NULL };
static char* watch_aliases[] = { "watch", "monitor", "observe", NULL };
static char* delete_aliases[] = { "del", "remove", "rm", NULL };

static struct {//这里进行了命令的别名修改
   char* name;
   char** aliases;  //新增别名数组
   char* description;
   int (*handler) (char*);
} cmd_table[] = {
  { "help",help_aliases, "Display informations about all supported commands", cmd_help },
  { "c",continue_aliases, "Continue the execution of the program", cmd_c },
  { "q",quit_aliases, "Exit NEMU", cmd_q },
  { "si",step_aliases, "Step through program by N instructions", cmd_si },
  { "info",info_aliases, "Print program status", cmd_info },
  { "x",scan_aliases,"Find the value of the expression EXPR, \
use the result as the starting memory address, \
and output N consecutive 4-byte outputs in hexadecimal.",cmd_x},
  {"p",print_aliases,"Find the value of the expression EXPR",cmd_p},
  {"w",watch_aliases,"Setting up monitoring points",cmd_w},
  {"d",delete_aliases,"Delete the monitoring point with serial number N",cmd_d},
   /* TODO: Add more commands */

};


static int cmd_c(char* args) {
   cpu_exec(-1);//uint64_t: -1   由于uint64_t是无符号的，当传入-1时，它会被转换为uint64_t能表示的最大值，即2^64 - 1。在实践中，这通常意味着"运行直到遇到停止条件"，而不是真的执行2^64 - 1条指令
   return 0;
}

static int cmd_q(char* args) {
   return -1;
}

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char* args) {
   /* extract the first argument */
   char* arg = strtok(NULL, " ");
   int i;

   if (arg == NULL) {
      /* no argument given */
      for (i = 0; i < NR_CMD; i++) {
         printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
      }
   }
   else {
      for (i = 0; i < NR_CMD; i++) {
         if (strcmp(arg, cmd_table[i].name) == 0) {
            printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
            return 0;
         }
      }
      printf("Unknown command '%s'\n", arg);
   }
   return 0;
}

static int cmd_si(char* args) {
   int step = 1;
   char* arg = strtok(NULL, " ");

   if (arg != NULL) {
      step = strtol(arg, NULL, 10);
      if (step < 1) {
         printf("Number of steps should be a positive integer\n");
         return -1;//表示出错
      }
   }
   cpu_exec(step);
   return 0;
}

static int cmd_info(char* args) {//info w监视点在之后pa实现到watchpoint.c中
   char* arg = strtok(NULL, " ");
   if (arg == NULL) {
      printf("Need more parameters.\n");
      return 0;
   }
   else if (*arg == 'r') {//r:register
      isa_reg_display();
   }
   else if (*arg == 'w') {//用于打印所有活跃监视点的信息
      WP* wp = get_head_wp();
      while (wp != NULL) {
         printf("Watchpoint %d: %s, value = %u\n", wp->NO, wp->expr, wp->new_value);
         wp = wp->next;
      }
   }
   return 0;
}

static int cmd_x(char* args) {
   int n = 1;  // 默认打印一个字
   char format = 'x';  // 默认以十六进制格式打印
   paddr_t addr;  // 用于存储解析出的地址

   char* arg = strtok(args, " ");
   if (arg == NULL) {
      printf("Missing number of words to print.\n");
      return -1;
   }

   n = strtol(arg, NULL, 10);  // 解析要打印的字数

   arg = strtok(NULL, " ");
   if (arg == NULL) {
      printf("Missing address.\n");
      return -1;
   }

   // 尝试解析格式选项（如果有的话）
   char* format_arg = strtok(NULL, " ");
   if (format_arg != NULL) {
      format = format_arg[0];  // 只取第一个字符作为格式
   }

   // 解析地址
   if (sscanf(arg, "%x", &addr) != 1) {
      printf("Invalid address.\n");
      return -1;
   }

   // 根据格式打印内存内容
   for (int i = 0; i < n; i++) {
      switch (format) {
      case 'x':  // 十六进制
         printf("0x%08x: 0x%08x\n", addr + 4 * i, vaddr_read(addr + 4 * i, 4));
         break;
      case 'd':  // 十进制
         printf("0x%08x: %u\n", addr + 4 * i, vaddr_read(addr + 4 * i, 4));
         break;
      case 'c':  // 字符
         printf("0x%08x: %c\n", addr + 4 * i, (char)vaddr_read(addr + 4 * i, 1));
         break;
         // 可以添加更多格式
      default:
         printf("Unknown format '%c'.\n", format);
         return -1;
      }
   }

   return 0;
}


static int cmd_p(char* args) {//求出表达式EXPR的值, EXPR支持的运算请见expr.c
   char* arg = strtok(args, " "); // 使用 args 获取第一个参数
   if (arg == NULL) {
      printf("Need more parameters.\n");
      return 0;
   }
   bool success = true;
   word_t result = expr(arg, &success);
   printf("%s = %d", arg, result);
   return result;
}

static int cmd_w(char* args) {
   if (args == NULL) {
      printf("Usage: w <expr>\n");
      return 0;
   }

   WP* wp = new_wp();
   strncpy(wp->expr, args, 255);  // 假设表达式长度不超过255
   wp->expr[255] = '\0';  // 确保字符串以空字符结尾

   bool success;
   wp->new_value = expr(wp->expr, &success);
   if (!success) {
      free_wp(wp);
      printf("Invalid expression.\n");
      return -1;
   }

   printf("Watchpoint %d: %s\n", wp->NO, wp->expr);
   return 0;
}

static int cmd_d(char* args) {//实现一个命令来删除特定的监视点
   int n;
   if (sscanf(args, "%d", &n) != 1) {
      printf("Usage: d <watchpoint number>\n");
      return -1;
   }

   // 假设有一个函数可以根据序号找到并删除监视点
   bool found = delete_watchpoint(n);
   if (!found) {
      printf("Watchpoint %d not found.\n", n);
      return -1;
   }

   printf("Deleted watchpoint %d\n", n);
   return 0;
}



void ui_mainloop() {

   init_wp_pool();//wp_pool链表池初始化: watchpoint功能初始化

   if (is_batch_mode()) {
      cmd_c(NULL);
      return;
   }

   for (char* str; (str = rl_gets()) != NULL; ) {
      char* str_end = str + strlen(str);

      /* extract the first token as the command */
      char* cmd = strtok(str, " ");
      if (cmd == NULL) { continue; }

      /* treat the remaining string as the arguments,
       * which may need further parsing
       */
      char* args = cmd + strlen(cmd) + 1;
      if (args >= str_end) {
         args = NULL;
      }

#ifdef HAS_IOE
      extern void sdl_clear_event_queue();
      sdl_clear_event_queue();
#endif
      int i = 0;//下方有适应别名数组的修改   
      for (; i < NR_CMD; i++) {
         if (strcmp(cmd, cmd_table[i].name) == 0) {
            if (cmd_table[i].handler(args) < 0) { return; }
            goto found_command;
         }

         char** alias = cmd_table[i].aliases;
         while (*alias) {
            if (strcmp(cmd, *alias) == 0) {
               if (cmd_table[i].handler(args) < 0) { return; }
               goto found_command;
            }
            alias++;
         }
      }
      printf("Unknown command '%s'\n", cmd);
   found_command:;

      if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
   }
}
