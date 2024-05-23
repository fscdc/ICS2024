#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
  // _switch(&pcb[i].as);
  // current = &pcb[i];
  // ((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}

// 0:pal, 1:hello, 2:videotest

int current_game = 0;

void update_current_game(){
	if(current_game == 0){
		current_game = 1;
	}else if(current_game == 1){
		current_game = 0;
	}else{
		assert(0);
	}
}

static int count=0;
#define FREQ 52
_RegSet* schedule(_RegSet *prev) {
  if(current!=NULL){
    current->tf=prev;
  }


  if(count==FREQ){
    current = &pcb[1];
    count = 0;
  } else {
    count++;
    current = (current_game == 0 ? &pcb[0] : &pcb[2]);
    // Log("The current_game is %d", current_game);
  }

  _switch(&current->as);
  return current->tf;
}
