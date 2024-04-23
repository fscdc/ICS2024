#include "common.h"
#include "syscall.h"

extern ssize_t fs_write(int fd, const void* buf, size_t len);
extern ssize_t fs_read(int fd, void* buf, size_t len);
extern size_t fs_filesz(int fd);
extern int fs_open(const char* pathname, int flags, int mode);
extern int fs_close(int fd);
extern off_t fs_lseek(int fd, off_t offset, int whence);

// uintptr_t sys_write(int fd, const void *buf, size_t count){
//   uintptr_t i = 0;
//   if(fd==1||fd==2){
//     for(;i<count;i++){
//       _putc(((char*)buf)[i]);
//     }
//   }
//   else{
//     return -1;
//   }
//   return i;
// }

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
    case SYS_brk:
      res = 0;
      break;
    case SYS_write: 
      res = fs_write(a[1], (void*)a[2], a[3]);
      break;
    case SYS_read:
      res = fs_read(a[1], (uint8_t*)a[2], a[3]);
      break;
    case SYS_open:
      res = fs_open((char*)a[1], a[2], a[3]);
      break;
    case SYS_close:
      res = fs_close(a[1]);
      break;
    case SYS_lseek:
        res = fs_lseek(a[1], a[2], a[3]);
        break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  SYSCALL_ARG1(r) = res;

  return NULL;
}
