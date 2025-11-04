
#ifndef MORSE_TABLE_H
#define MORSE_TABLE_H

#include <stddef.h>
#define MORSE_TABLE_BUFFER_SIZE 128
#define MAX_SCORE 99

struct morse_entry {
  const char *code;
  int score;
};

struct morse_entry **init_morse_table();
int table_size();
size_t table_max_code_length();
void uninit_morse_table();

#endif
