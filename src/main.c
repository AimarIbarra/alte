#include "term.h"
#include "die.h"
#include "stddef.h"

int main(void) {
  init_term();
  int file = open_file("*blank*");
  if (file < 0)
    die(NULL);
  switch_buff(file);
  const char hello[] = "Hello World!\r\n";
  buff_write(file, sizeof(hello) - 1, hello);
  refresh();

  return 0;
}
