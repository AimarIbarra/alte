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

  unsigned short win_w = term_width();
  unsigned short win_h = term_height();
  printf("window dimensions: { %hu, %hu }", win_w, win_h);

  while (1) {
    unsigned short cur_w = term_width();
    unsigned short cur_h = term_height();
    if (cur_w != win_w || cur_h != win_h) {
      printf("win rsiz:{%hu,%hu}", cur_w, cur_h);
      win_w = cur_w;
      win_h = cur_h;
    }

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

