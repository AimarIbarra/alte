#include <stdio.h>
#include "output.h"
#include "input.h"
#include "term.h"

int main(void) {
  const char *err = init_term();
  if (err) {
    perror(err);
    perror(NULL);
    return 1;
  }
  set_scroll();

  while (1) {
    unsigned char in = read_ch();
    switch (in) {
    case NO_INPUT:
      break;
    case ALT_KEY:
      print("<M-%c>", get_special_key());
      break;
    case MOVE_KEY:
      move_cursor(get_special_key());
      break;
    case 'q':
      quit_term();
      return 0;
    case '\r':
      newline();
      break;
    case DEL:
      backspace();
      break;
    default:
      print("%c", in);
      break;
    }
  }
}

