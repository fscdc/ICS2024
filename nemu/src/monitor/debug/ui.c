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

  /* TODO: Add more commands */
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
    
    // If no argument is provided, execute one instruction by default.
    if(arg == NULL){
        cpu_exec(1);
    }
    else{
        // Convert the argument to an integer to get the number of instructions to execute.
        int num = atoi(arg);
        
        // Check if the number is valid (non-negative).
        if(num < 0){
            printf("Invalid args, try again.\n");
            return 0;
        }
        
        // Ensure there are no additional arguments.
        if(strtok(NULL, " ") != NULL){
            printf("Too many args, try again!\n");
            return 0;
        }
        
        // Execute the specified number of instructions.
        cpu_exec(num);
    }
    return 0;
}


static int cmd_info(char* args){
    char* arg = strtok(args, " ");
    
    // Check if the subcommand is provided.
    if(arg == NULL){
        printf("Subcommand is required.\n");
    }
    else{
        // Ensure that only one argument is provided.
        if(strtok(NULL, " ") != NULL){
            printf("Too many args, try again!\n");
            return 0;
        }
        
        // Handle the 'r' subcommand to display register values.
        if(strcmp(arg, "r") == 0){
            printf("eax: 0x%08x.\n", cpu.eax);
            printf("ecx: 0x%08x.\n", cpu.ecx);
            printf("edx: 0x%08x.\n", cpu.edx);
            printf("ebx: 0x%08x.\n", cpu.ebx);
            printf("esp: 0x%08x.\n", cpu.esp);
            printf("ebp: 0x%08x.\n", cpu.ebp);
            printf("esi: 0x%08x.\n", cpu.esi);
            printf("edi: 0x%08x.\n", cpu.edi);
            printf("eip: 0x%08X\n", cpu.eip);
        }
        // Handle the 'w' subcommand to display watchpoint information.
        else if(strcmp(arg, "w") == 0){
            print_wp();
        }
        // Handle unknown subcommands.
        else{
            printf("Unknown subcommand '%s'.\n", arg);
        }
    }
    return 0;
}


static int cmd_p(char *args) {
  // Check if arguments are provided.
  if (args == NULL) {
    printf("Args is required.\n");
    return 0;
  }
  
  bool success;
  
  // Evaluate the expression passed as arguments.
  uint32_t result = expr(args, &success);
  
  // If the expression is successfully evaluated, print the result.
  if (success) {
    printf("Result = %d\n", result);
  } else {
    printf("Failed to evaluate the expression.\n");
  }
  
  return 0;
}


static int cmd_x(char* args) {
    char* arg = strtok(args, " ");
    if(arg == NULL) {
        printf("Args is required.\n");
        return 0;
    }

    bool success;
    int num = atoi(arg);
    
    arg = strtok(NULL, " ");
    if(arg == NULL) {
        printf("Error: Missing memory address.\n");
        return 0;
    }

    if(strtok(NULL, " ") != NULL) {
        printf("Too many args, try again!\n");
        return 0;
    }

    vaddr_t vaddr = expr(arg, &success); // Evaluates the expression for the memory address.
    if(!success) {
        printf("Error: Invalid memory address.\n");
        return 0;
    }

    // Iterate through the specified number of memory locations, reading and printing each.
    for(int i = 0; i < num; i++) {
        uint32_t data = vaddr_read(vaddr + 4 * i, 4);
        printf("Memory[0x%08x]: ", vaddr + 4 * i);
        for(int j = 0; j < 4; j++) {
            printf("%02x ", (data >> (8 * j)) & 0xff);
        }
        printf("\n");
    }
    return 0;
}


static int cmd_w(char* args){
    // Check if the argument for the watchpoint expression is provided.
    if (args == NULL) {
        printf("Args is required.\n");
        return 0;
    }

    // Concatenate all arguments into a single expression string.
    char expression[256] = "";
    char* token = strtok(args, " ");
    while (token != NULL) {
        strcat(expression, token);
        token = strtok(NULL, " ");
    }

    // Initialize a new watchpoint.
    bool success;
    WP* wp = new_wp();
    wp->expr = strdup(expression); // Use strdup to duplicate the expression string.
    wp->value = expr(wp->expr, &success);

    if (!success) {
        printf("Error: Invalid expression for watchpoint.\n");
        free_wp(wp->NO); // Clean up the newly created but invalid watchpoint.
        return 0;
    }

    printf("Watchpoint %d set on %s.\n", wp->NO, wp->expr);
    return 0;
}


static int cmd_d(char* args) {
  char* arg = strtok(args, " ");
  
  // If no argument is provided, notify the user and exit the command.
  if(arg == NULL) {
    printf("Too few args.\n");
    return 0;
  }
  
  // If there's more than one argument, notify the user and exit the command.
  if(strtok(NULL, " ") != NULL) {
    printf("Too many args.\n");
    return 0;
  }
  
  // Evaluate the argument as an expression to get the watchpoint number.
  bool success;
  int n = expr(arg, &success); 
  
  // Validate the obtained watchpoint number.
  if (n < 0 || n >= 32) { 
    printf("Error: Invalid watchpoint number.\n");
    return 0; 
  }
  
  // Proceed to delete the watchpoint with the obtained number.
  free_wp(n); 
  printf("Watchpoint %d deleted successfully.\n", n);
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
