#include <isa.h>
#include "expr.h"
#include "watchpoint.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

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

static int cmd_c(char* args) {
   cpu_exec(-1);//uint64_t: -1   由于uint64_t是无符号的，当传入-1时，它会被转换为uint64_t能表示的最大值，即2^64 - 1。在实践中，这通常意味着"运行直到遇到停止条件"，而不是真的执行2^64 - 1条指令
   return 0;
}


static int cmd_q(char* args) {
   return -1;
}

static int cmd_si(char* args);

static int cmd_help(char* args);

static struct {
   char* name;
   char* description;
   int (*handler) (char*);
} cmd_table[] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Step through program by N instructions", cmd_si },
  { "info", "Print program status", cmd_info },
  /* TODO: Add more commands */

};

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
   if (args != NULL) {
      step = strtol(args, NULL, 10);
      if (step < 1) {
         printf("Number of steps should be a positive integer\n");
         return -1;//表示出错
      }
   }
   cpu_exec(step);
   return 0;
}

void ui_mainloop() {
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

      int i;
      for (i = 0; i < NR_CMD; i++) {
         if (strcmp(cmd, cmd_table[i].name) == 0) {
            if (cmd_table[i].handler(args) < 0) { return; }
            break;
         }
      }

      if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
   }
}
