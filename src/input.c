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

static char special_key;

char get_special_key(void) {
  return special_key;
}

unsigned char read_ch(void) {
  buff_len = read(STDIN_FILENO, buff, sizeof(buff));
  if (buff_len == 0)
    return NO_INPUT;

  if (buff_len == 1) {
    return buff[0];
  }

  if (is_move()) {
    //            \x1b[c
    special_key = buff[2];
    return MOVE_KEY;
  }

  if (is_alt()) {
    //                [c
    special_key = buff[1];
    return ALT_KEY;
  }

  return NO_INPUT;
}

