#include <stdio.h>
#include "input.h"
#include "term.h"

int main(void) {
  const char *err = init_term();
  if (err) {
    perror(err);
    perror(NULL);
    return 1;
  }

  while (1) {
    unsigned char in = read_ch();
    switch (in) {
    case 0xff:
      continue;
    case 'q':
      quit_term();
      return 0;
    default:
      printf("%c", (char)in);
    }
  }
}

