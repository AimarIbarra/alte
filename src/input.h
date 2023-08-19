#ifndef INPUT_H
#define INPUT_H

enum utf_8_unused_values {
  DEL            = 127,
  NO_INPUT       = 0xc0,
  ALT_KEY        = 0xc1,
  MOVE_KEY       = 0xfe,
  UTF_8_UNUSED_4 = 0xff,
};

unsigned char read_ch(void);
char get_special_key(void);

#endif

