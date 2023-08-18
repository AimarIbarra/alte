#include <sys/ioctl.h>
#include <sys/stat.h>
#include <asm/termbits.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "die.h"
#include "term.h"

#define ERR(str) \
  write(STDERR_FILENO, str "\n", sizeof(str))

// 65535 is the biggest value in a unsigned short
#define US_MAX_LEN 5

static char *put_us(char *buff, unsigned short n) {
  while (n) {
    unsigned short r = n % 10;
    n /= 10;
    *buff = r + '0';
    ++buff;
  }
  return buff;
}

static size_t default_buffer_size = 512;

struct cursor {
  unsigned short x;
  unsigned short y;
};

enum buffer_state {
  buffer_unused,
  buffer_saved,
  buffer_unsaved,
  buffer_temp,
};

struct buffer {
  size_t size;
  size_t capacity;
  char *data;
  char *name;
  const char *showptr;
  char *writeptr;
  int showcount;
  int fd;
  enum buffer_state state;
  struct cursor cur;
};

struct window {
  unsigned short w;
  unsigned short h;
};

#define INITIAL_BUFFERS 4

static struct {
  int open_buff;
  int buff_count;
  int max_buffs;
  struct buffer *buffs;
  struct window win;
} terminal = {.buff_count = INITIAL_BUFFERS};

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

static void quit_term(void) {
  if (terminal.buffs) {
    for (int i = 0; i < terminal.buff_count; ++i) {
      close_buff(i);
    }
    free(terminal.buffs);
  }
  const char ctrl[] = "\x1b[r\x1b[?1049l";
  write(STDOUT_FILENO, ctrl, sizeof(ctrl) - 1);
  if (ioctl(STDIN_FILENO, SET, &oldcfg) < 0)
    ERR("Couldn't restore terminal settings.\n"
        "Please consider restarting "
        "your environment.");
}

void init_term(void) {
  if (ioctl(STDIN_FILENO, GET, &oldcfg) < 0)
    die("Couldn't get stdin configuration.");
  if (atexit(&quit_term) != 0)
    die("Couldn't register cleanup.");

  Tio newcfg = oldcfg;
  newcfg.c_oflag &= ~OPOST;
  newcfg.c_lflag &= ~(ECHO | ISIG | ICANON);
  newcfg.c_iflag &= ~ICRNL;
  newcfg.c_cc[VMIN] = 0;
  newcfg.c_cc[VTIME] = 1;

  if (ioctl(STDIN_FILENO, SET, &newcfg) < 0)
    die("Couldn't change stdin configuration.");

  struct winsize sz;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &sz) < 0)
    die("Couldn't get terminal size.");
  terminal.win.w = sz.ws_col;
  terminal.win.h = sz.ws_row;

  terminal.buffs = calloc(sizeof(struct buffer), INITIAL_BUFFERS);
  if (!terminal.buffs)
    die(NULL);

  char ctrl[sizeof("\x1b[?1049h\x1b[;r") - 1 + US_MAX_LEN] =
    {0x1b,'[','?','1','0','4','9','h',0x1b,'[','1',';'};
  char * const p =
    ctrl + sizeof("\x1b[?1049h\x1b[1;") - 1;
  char * const a = put_us(p, sz.ws_row);
  write(STDOUT_FILENO, ctrl, a - ctrl);
}

// TODO: Reuse unused buffers
int open_file(const char *name) {
  for (int i = 0; i < terminal.buff_count; ++i) {
    if (terminal.buffs[i].state == buffer_unused)
      continue;
    if (strcmp(name, terminal.buffs[i].name) == 0) {
      return i;
    }
  }

  if (terminal.buff_count == terminal.max_buffs) {
    terminal.max_buffs <<= 1;
    void *tmp =
      realloc(terminal.buffs,
              sizeof(struct buffer[terminal.max_buffs]));
    if (!tmp)
      die(NULL);
    terminal.buffs = tmp;
  }

  struct buffer *buf =
    &terminal.buffs[terminal.buff_count];

  int fd = open(name, O_RDONLY);
  if (fd < 0) {
    if (errno == ENOENT) {
      buf->state = buffer_temp;
      buf->size = 0;
      buf->capacity = default_buffer_size;
      buf->data = malloc(default_buffer_size);
      if (!buf->data) {
        ERR("Couldn't create buffer.");
        return -1;
      }
      buf->showptr = buf->data;
      buf->writeptr = buf->data;
      buf->showcount = 0;
      buf->name = strdup(name);
      if (!buf->name) {
        ERR("Couldn't create buffer.");
        free(buf->data);
        return -1;
      }
      buf->cur.x = 0;
      buf->cur.y = 0;
      return terminal.buff_count++;
    }
    ERR("Couldn't open file.");
    return -1;
  }

  struct stat statbuf;
  if (fstat(fd, &statbuf) < 0) {
    ERR("Couldn't stat file.");
    close(fd);
    return -1;
  }

  buf->state = buffer_saved;
  buf->size = statbuf.st_size;
  buf->capacity = statbuf.st_size;
  buf->data = malloc(statbuf.st_size);
  if (!buf->data) {
    ERR("Couldn't create buffer.");
    close(fd);
    return -1;
  }
  buf->cur.x = 0;
  buf->cur.y = 0;
  buf->showptr = buf->data;
  buf->showcount =
    statbuf.st_size < terminal.win.w * terminal.win.h ?
    statbuf.st_size : terminal.win.w * terminal.win.h;
  buf->name = strdup(name);
  if (!buf->name) {
    ERR("Couldn't create buffer.");
    free(buf->data);
    close(fd);
    return -1;
  }

  if (read(fd, buf->data, statbuf.st_size) != statbuf.st_size) {
    ERR("Couldn't read contents of file.");
    free(buf->data);
    free(buf->name);
    close(fd);
    return -1;
  }

  close(fd);
  return terminal.buff_count++;
}

void refresh(void) {
  struct buffer *buf =
    &terminal.buffs[terminal.open_buff];

  write(STDOUT_FILENO, buf->showptr, buf->showcount);
}

int buff_write(int id,
               unsigned int size,
               const char str[size]) {
  struct buffer *buf = &terminal.buffs[id];
  if (buf->capacity < buf->size + size) {
    buf->capacity <<= 1;
    void *tmp = realloc(buf->data, buf->capacity);
    if (!tmp) {
      ERR("Couldn't realloc buffer.");
      close_buff(id);
      return -1;
    }
    buf->data = tmp;
  }
  const char *endptr = buf->data + buf->size;
  const size_t to_write = endptr - buf->writeptr;
  memmove(buf->writeptr + size, buf->writeptr, to_write);
  memcpy(buf->writeptr, str, size);
  buf->showcount += size;
  return 0;
}

int save_buff(int id) {
  struct buffer *buf = &terminal.buffs[id];

  switch (buf->state) {
  case buffer_saved:
    return 0;
  case buffer_unused:
    return -1;
  case buffer_temp:
    buf->fd = open(buf->name, O_WRONLY | O_CREAT);
    if (buf->fd < 0) {
      return -1;
    }
  case buffer_unsaved:
    write(buf->fd, buf->data, buf->size);
    return 0;
  }
}

void close_buff(int id) {
  struct buffer *buf = &terminal.buffs[id];

  free(buf->name);
  free(buf->data);
  buf->state = buffer_unused;
}

int switch_buff(int id) {
  terminal.open_buff = id;
  refresh();
  return 0;
}
