#include <unistd.h>
#include <stdio.h>

unsigned char read_ch(void) {
  static unsigned char buff[16];
  ssize_t in =
    read(STDIN_FILENO, buff, 16);
  if (in == 0) {
    return 0xff;
  }
  if (in == 1) {
    return buff[0];
  }

  for (int i = 0; i < in; ++i) {
    if (buff[i] == 0x1b) {
      printf("\\x1b");
      continue;
    }
    putchar(buff[i]);
  }
  return 0xff;
}

