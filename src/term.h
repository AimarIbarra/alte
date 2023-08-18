#ifndef TERM_H
#define TERM_H

void init_term(void);
int open_file(const char *name);
int buff_write(int id,
               unsigned int size,
               const char str[size]);
int open_buff(void);
int switch_buff(int id);
int save_buff(int id);
void close_buff(int id);
void refresh(void);
const char *buff_name(int id);

#endif
