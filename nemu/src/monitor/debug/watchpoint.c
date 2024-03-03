#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp() {
    if (free_ == NULL) {
        printf("No available space.\n");
        assert(0);
    }

    // Allocate a watchpoint from the free list.
    WP* newWatchpoint = free_;
    free_ = free_->next; // Move the head of the free list to the next element.

    // Initialize the allocated watchpoint.
    newWatchpoint->next = NULL; 
    newWatchpoint->used = true; 

    // If the active list is empty, this watchpoint becomes the new head.
    if (head == NULL) {
        head = newWatchpoint;
    } else {
        // If the active list is not empty, append the new watchpoint to the end of the list.
        WP* current = head;
        while (current->next != NULL) {
            current = current->next; // Traverse to the last watchpoint in the active list.
        }
        current->next = newWatchpoint; 
    }

    return newWatchpoint;
}


void free_wp(int no){
  WP* curr_head = head;
  WP* curr_free = free_;
  if (0 > no || no >= 32) {
    printf("Error: %d is not an acceptable watchpoint.\n", no);
  }
  if (curr_head == NULL || (curr_head->next == NULL && curr_head->NO != no)) {
    printf("Error: try to free a nonexistent watchpoint.\n");
    assert(0);
  }
  if (curr_head->NO == no) {
    printf("%d\n", no);
    while(curr_free->next != NULL) {
      curr_free = curr_free->next;
    }
    curr_free->next = curr_head;
    if (curr_head->next != NULL) {
      head = curr_head->next;
    }
    else {
      head = NULL;
    }
    curr_free->next->used = false;
    curr_free->next->expr = NULL;
    curr_free->next->next = NULL;
    
    return;
  }
  if (curr_free == NULL) {
    while(curr_head != NULL && curr_head->next->NO != no) {
      curr_head = curr_head->next;
    }
    if (curr_head == NULL) {
      printf("Error: watchpoint %d is not found to be used.\n", no);
    }
    curr_head->next->used = false;
    curr_head->next->expr = NULL;
    curr_free = curr_head->next;
    curr_head->next = curr_head->next->next;
    curr_free->next = NULL;
  }
  else {
    while(curr_head != NULL && curr_head->next->NO != no) {
      curr_head = curr_head->next;
    }
    while(curr_free->next != NULL) {
      curr_free = curr_free->next;
    }
    if (curr_head == NULL) {
      printf("Error: watchpoint %d is not found to be used.\n", no);
    }
    curr_head->next->used = false;
    curr_head->next->expr = NULL;
    curr_free->next = curr_head->next;
    curr_head->next = curr_head->next->next;
    curr_free->next->next = NULL;
  }
}

void print_wp() {
  WP* curr = head;
  if (head == NULL) {
    printf("No watchpoints.\n");
  }
  while (curr != NULL) {
    printf("Watchpoint:  %d:\t%s\t\t%d\n", curr->NO, curr->expr, curr->value);
    curr = curr->next;
  }
}

bool check_wp() {
  if (head == NULL) {
    return false;
  }

  bool changed = false; // Flag to indicate if any watchpoint's value has changed.
  WP* curr = head; // Start from the first active watchpoint.

  while (curr != NULL) {
    bool success;
    int newVal = expr(curr->expr, &success); // Evaluate the current expression's value.

    // If evaluation succeeded and the value has changed, update the watchpoint.
    if (success && newVal != curr->value) {
      printf("Watchpoint %d: '%s' changed from %d to %d.\n", curr->NO, curr->expr, curr->value, newVal);
      curr->value = newVal; // Update the watchpoint's value to the new value.
      changed = true; // Set the flag to true as at least one watchpoint has changed.
    }

    curr = curr->next; 
  }

  return changed;
}

