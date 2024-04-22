#include "common.h"
#include "fs.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

extern void ramdisk_read(void* buf, off_t offset, size_t len);
extern void ramdisk_write(const void* buf, off_t offset, size_t len);
extern size_t get_ramdisk_size();

extern int fs_open(const char *pathname, int flags, int mode);
extern size_t fs_filesz(int fd);
extern size_t fs_read(int fd, void *buf, size_t len);
extern int fs_close(int fd);

uintptr_t loader(_Protect *as, const char *filename) {
  // impl loader--fsc
  // ramdisk_read(DEFAULT_ENTRY, 0, get_ramdisk_size());
  // return (uintptr_t)DEFAULT_ENTRY;
  filename = "/bin/events";
  int fd = fs_open(filename, 0, 0);
  printf("fd to open = %d\n", fd);
  fs_read(fd, DEFAULT_ENTRY, fs_filesz(fd));
  fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
