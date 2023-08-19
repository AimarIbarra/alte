#include <stdio.h>
#include "codes.h"
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
    case NO_INPUT:
      continue;
    case 'q':
      quit_term();
      return 0;
    case '\r':
      printf("\r\n");
      break;
    case '\b':
      printf(CUR_LEFT CLR_EOL);
      // TODO: Reprint rest of line
      break;
    default:
      printf("%c", (char)in);
      // TODO: When the cursor isn't at the end of
      //       the line, redraw rest of line.
      break;
    }
  }
}

