#include <stdio.h>
#include <stdlib.h>

__attribute__((noreturn))
void die(const char *msg) {
  perror(msg);
  exit(1);
}
