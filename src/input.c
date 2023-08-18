#include <unistd.h>

unsigned char read_ch(void) {
  static unsigned char buff[16];
  ssize_t in =
    read(STDIN_FILENO, buff, 16);
  if (in != 1) {
    return 0xff;
  }
  return buff[0];
}

