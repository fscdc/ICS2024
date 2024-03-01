#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

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

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

//add func
static int cmd_si(char *args); 

static int cmd_info(char* args); 

static int cmd_p(char *args);

static int cmd_x(char *args); 

static int cmd_w(char* args);

static int cmd_d(char* args); 


static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands --finish*/
  {"si", "Single-step debug(args:N)", cmd_si},
  {"info", "Print information of reg or watchpoint(args:r/w)", cmd_info},
  {"p", "Calculate the expr(args:expr)", cmd_p},
  {"x", "Scan the memory(args:N expr)", cmd_x},
  {"w", "Set appointed watchpoint(args:expr)", cmd_w},
  {"d", "Delete appointed watchpoint(args:N)", cmd_d}
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char* args){
	char* arg = strtok(args, " ");
	if(arg==NULL){
		cpu_exec(1);
	}
	else{
		bool success;
		int num = expr(arg, &success);
		if(!success){
			printf("Invalid si num.\n");
			return 0;
		}
		if(strtok(NULL," ") != NULL){
			printf("Too many arguments!\n");
			return 0;
		}
		cpu_exec(num);
	}
	return 0;
}

static int cmd_info(char* args){
	char* arg = strtok(args, " ");
	if(arg == NULL){
		printf("Too few arguments.\n");
	}
	else{
		if(strtok(NULL, " ") != NULL){
			printf("Too many arguments.\n");
			return 0;
		}
	if(strcmp(arg,"r")==0){
		printf("eax: 0x%08x.\n", cpu.eax);
		printf("ecx: 0x%08x.\n", cpu.ecx);
		printf("edx: 0x%08x.\n", cpu.edx);
		printf("ebx: 0x%08x.\n", cpu.ebx);
		printf("esp: 0x%08x.\n", cpu.esp);
		printf("ebp: 0x%08x.\n", cpu.ebp);
		printf("esi: 0x%08x.\n", cpu.esi);
		printf("edi: 0x%08x.\n", cpu.edi);
		printf("NEMU eflags:\n");
		printf("ZF:%d\n",cpu.eflags.ZF);
		printf("SF:%d\n",cpu.eflags.SF);
		printf("OF:%d\n",cpu.eflags.OF);
		printf("CF:%d\n",cpu.eflags.CF);
		printf("IF:%d\n",cpu.eflags.IF);
	}
	else if(strcmp(arg,"w")==0){
		show_wp();
	}
	}
	return 0;
}

static int cmd_p(char *args) {
  uint32_t i;
  bool success;


  if (args == NULL) {
    printf("Exception: Expression field EXPR is required.\n");
  }
  else {
    i = expr(args, &success, false);
    if (success)
      printf("Result = %d\n       = 0x%08X\n", i, i);
  }
  return 0;
}


static int cmd_x(char* args){
	char* arg = strtok(args, " ");
	if(arg == NULL){
		printf("Too few arguments!\n");
	}
	bool success;
	int num = expr(arg, &success);
	if(!success){
		printf("Invalid cmd_x num.\n");
		return 0;
	}
	arg = strtok(NULL, " ");
	if(arg == NULL){
		printf("Too few arguments!\n");
		return 0;
	}
	if(strtok(NULL," ") != NULL){
		printf("Too many arguments!\n");
		return 0;
	}
	vaddr_t vaddr = expr(arg, &success);
	if(!success){
		printf("Invalid cmd_x address!\n");
		return 0;
	}
	for(int i = 0; i < num; i++){
		uint32_t data = vaddr_read(vaddr + 4 * i, 4);
		printf("0x%08x ", vaddr + 4 * i);
		for(int j = 0; j < 3; j++){
			printf("0x%02x ", data & 0xff);
			data = data >> 8;
		}
		printf("0x%02x\n", data & 0xff);
	}
	return 0;
}

static int cmd_w(char* args){
  char* arg = strtok(args, " ");
  if (arg == NULL) {
    printf("Too few arguments!\n");
    return 0;
  }
  char* sub = strtok(NULL, " ");
  while (sub != NULL) {
    strcat(arg, sub);
    sub = strtok(NULL, " ");
  }
  bool success;
  WP* wp = new_wp();
  // printf("%d\n", wp->NO);
  wp->expr = (char*)malloc(strlen(arg)*sizeof(char));
  memset(wp->expr, 0, strlen(arg));
  strcpy(wp->expr, arg);
  // printf("%s\n", wp->expr);
  wp->value = expr(arg, &success);
  printf("Set a watchpoint %d on %s.\n", wp->NO, wp->expr);
  return 0;

}

static int cmd_d(char* args) {
  char* arg = strtok(args, " ");
  if(arg == NULL) {
    printf("Too few arguments.\n");
    return 0;
  }
  if(strtok(NULL, " ") != NULL) {
    printf("Too many arguments.\n");
    return 0;
  }
  bool success;
  int n = expr(arg, &success);
  if (n >= 32) {
    printf("Only 32 watchpoints(No: 0-31) provided.\n");
    return 0;
  }
  free_wp(n);
  printf("Successfully delete watchpoint %d.\n", n);
  return 0;
}


//chief func used for implement "user interaction" 
void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
