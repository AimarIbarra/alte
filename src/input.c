#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "input.h"

#define ASCII_DEL 127

// For now 16 sounds like a good number
static unsigned char buff[16];
static ssize_t buff_len;

static bool is_alt(void) {
  if (buff_len != strlen("\033a"))
    return false;
  return buff[0] == '\x1b';
}

static bool is_move(void) {
  if (buff_len != strlen("\x1b[A"))
    return false;
  return memcmp(buff, "\x1b[", buff_len - 1) == 0
         && (buff[2] <= 'D' || buff[2] >= 'A');
}

unsigned char read_ch(void) {
  buff_len = read(STDIN_FILENO, buff, sizeof(buff));
  if (buff_len == 0)
    return NO_INPUT;

  if (buff_len == 1) {
    // Output is raw so we better send a backspace
    if (buff[0] == ASCII_DEL)
      return '\b';

    return buff[0];
  }

  if (is_move())
    write(STDOUT_FILENO, buff, buff_len);
  if (is_alt())
    printf("<Alt-%c>", buff[1]);

  return NO_INPUT;
}

