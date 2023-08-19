#ifndef TERM_H
#define TERM_H

unsigned short term_width(void);
unsigned short term_height(void);

const char *init_term(void);
const char *quit_term(void);

#endif
