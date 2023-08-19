#include <sys/ioctl.h>
#include <asm/termbits.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#ifdef TEGETS2
typedef struct termios2 Tio;
#define GET TCGETS2
#define SET TCSETS2
#else
typedef struct termios Tio;
#define GET TCGETS
#define SET TCSETS
#endif

static Tio oldcfg;

const char *quit_term(void) {
  const char ctrl[] = "\x1b[r"
                      "\x1b[?1049l";
  write(STDOUT_FILENO, ctrl, sizeof(ctrl) - 1);
  if (ioctl(STDIN_FILENO, SET, &oldcfg) < 0)
    return "Couldn't restore terminal settings.";
  return NULL;
}

const char *init_term(void) {
  setvbuf(stdout, NULL, _IONBF, 0);
  char *term = getenv("TERM");
  if (!term)
    return "Couldn't get terminal name.";
  if (strcmp(term, "xterm")
      && strcmp(term, "xterm-256color"))
    return "Unsupported terminal.";

  if (ioctl(STDIN_FILENO, GET, &oldcfg) < 0)
    return "Couldn't get stdin configuration.";

  Tio newcfg = oldcfg;
  newcfg.c_oflag &= ~OPOST;
  newcfg.c_lflag &= ~(ECHO | ISIG | ICANON);
  newcfg.c_iflag &= ~ICRNL;
  newcfg.c_cc[VMIN] = 0;
  newcfg.c_cc[VTIME] = 1;
  if (ioctl(STDIN_FILENO, SET, &newcfg) < 0)
    return "Couldn't change stdin configuration.";

  struct winsize sz;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &sz) < 0)
    return "Couldn't get terminal size.";

  printf("\x1b[?1049h"
         "\x1b[1;%hur", sz.ws_row);
  return NULL;
}
