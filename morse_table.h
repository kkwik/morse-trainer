
#ifndef MORSE_TABLE_H
#define MORSE_TABLE_H

#include <stddef.h>
#define MORSE_TABLE_LENGTH 128
#define MAX_SCORE 100

struct morse_entry {
  char *code;
  int score;
};

size_t max_code_length(struct morse_entry **);
struct morse_entry **init_morse_table();
void uninit_morse_table(struct morse_entry **);

#endif
