
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
void uninit_morse_table();
int get_table_entry_count();
size_t get_table_max_code_length();

#endif
