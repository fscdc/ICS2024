#include "common.h"
#include "syscall.h"

extern ssize_t fs_write(int fd, const void* buf, size_t len);

uintptr_t sys_write(int fd, const void *buf, size_t count){
  uintptr_t i = 0;
  if(fd==1||fd==2){
    for(;count>0;count--){
      _putc(((char*)buf)[i]);
      i++;
    }
  }
  return 3;
}

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);  

  uintptr_t res = -1;

  switch (a[0]) {
    case SYS_none: 
        res = 1;
        break;
    case SYS_exit:
        _halt(a[1]);
        break;
    case SYS_write: 
        res = sys_write(a[1], (void*)a[2], a[3]);
        break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  SYSCALL_ARG1(r) = res;

  return NULL;
}
