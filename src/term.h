#ifndef TERM_H
#define TERM_H

#include <stdbool.h>

bool window_resized(void);
unsigned short term_width(void);
unsigned short term_height(void);

const char *init_term(void);
void set_scroll(void);
const char *quit_term(void);

#endif
