#include <am.h>
#include <x86.h>

#define RTC_PORT 0x48   // Note that this is not standard
static unsigned long boot_time;

void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
  unsigned long ms = inl(RTC_PORT) -  boot_time;
  return ms;
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
  int len = sizeof(uint32_t);
  for(int i = 0; i < h; i++) {
      memcpy(fb + (y + i) * _screen.width + x, pixels + i * w, len * w);
  }
}

void _draw_sync() {
}

int _read_key() {
  uint32_t code = _KEY_NONE;
  if (inb(0x64) == (uint8_t)1) {
    code = inl(0x60);
  }
  return code;
}
