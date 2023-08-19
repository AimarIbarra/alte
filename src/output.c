#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include "codes.h"
#include "term.h"
#include "output.h"

#define WRITE1(d, s) write(STDOUT_FILENO, d, s)
#define WRITE(d) WRITE1(&d, sizeof(d))

// TODO: When overflowing, just write.
static char buff[16];
#define ASSERT_OVERFLOW(p) \
  assert((p) - buff < 16 && "Overflow when printing!")

static unsigned int col;

static char *append_line(char *p) {
  if (col >= term_width()) {
    const char msg[5] = "\r\n ~>";
    ASSERT_OVERFLOW(p + 5);
    col = 3;
    return memcpy(p, msg, 5) + 5;
  }
  return p;
}

void print(const char * restrict f, ...) {
  va_list ap;
  va_start(ap, f);

  char *write_ptr = buff;
  while (f[0] != '\0') {
    write_ptr = append_line(write_ptr);
    ASSERT_OVERFLOW(write_ptr);
    if (f[0] == '%') {
      ++f;
      if (f[0] == 'c') {
        *write_ptr = va_arg(ap, int);
        ++write_ptr;
        ++col;

      } else {
        assert(0 && "Invalid format value.");
      }

    } else {
      *write_ptr = f[0];
      ++write_ptr;
      ++col;
    }
    ++f;
  }
  va_end(ap);
  WRITE1(buff, write_ptr - buff);
}

void newline(void) {
  col = 0;
  const char msg[2] = "\r\n";
  WRITE(msg);
}

void backspace(void) {
  if (col) {
    --col;
    const char msg[sizeof(CLR_EOL)] = "\b" CLR_EOL;
    WRITE(msg);

  } else {
    // TODO: Set col to end of prev line
    const char msg[sizeof(CLR_EOS CUR_UP) - 1] =
      CLR_EOS CUR_UP;
    WRITE(msg);
  }
}

#define WCSTR(s) WRITE1(s, sizeof(s) - 1)

void move_cursor(char c) {
  switch (c) {
  case 'A':
    WCSTR(CUR_UP);
    break;
  case 'B':
    WCSTR(CUR_DOWN);
    break;
  case 'C':
    ++col;
    WCSTR(CUR_RIGHT);
    break;
  case 'D':
    if (col) {
      --col;
      WCSTR(CUR_LEFT);
    }
    break;
  default:
    __builtin_unreachable();
  }
}

