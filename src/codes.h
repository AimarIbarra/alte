#ifndef CODES_H
#define CODES_H

// Since we are using the window size, and the
// kernel reports window dimensions with the
// following structure:
//   struct winsize {
//     unsigned short ws_row;
//     unsigned short ws_col;
//     // more variables
//   };
// all the format macros that take numbers are
// of %hu format (unsigned short).

// TODO: Other clearing escape sequences...
#define CLR_SCREEN "\x1b[2J"
#define CLR_EOL "\x1b[K"
#define CLR_EOS "\x1b[1J"

// TODO: There are control codes that move the
//       cursor just like the following ones,
//       but will also change the column to 1.
#define CUR_UP "\x1b[A"
#define CUR_UP_N(n) "\x1b[" #n "A"
#define CUR_UP_N_FMT "\x1b[%huA"
#define CUR_DOWN "\x1b[B"
#define CUR_DOWN_N(n) "\x1b[" #n "B"
#define CUR_DOWN_N_FMT "\x1b[%huB"
#define CUR_RIGHT "\x1b[C"
#define CUR_RIGHT_N(n) "\x1b[" #n "C"
#define CUR_RIGHT_N_FMT "\x1b[%huC"
#define CUR_LEFT "\x1b[D"
#define CUR_LEFT_N(n) "\x1b[" #n "D"
#define CUR_LEFT_N_FMT "\x1b[%huD"

#define CUR_TO(x, y) "\x1b[" #y ";" #x "H"
// The format is (y, x).
#define CUR_TO_FMT "\x1b[%hu;%huH"

#endif

