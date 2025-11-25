
#ifndef MORSE_TABLE_H
#define MORSE_TABLE_H

#include <stdbool.h>
#include <stddef.h>
#define MORSE_TABLE_BUFFER_SIZE 128
#define MAX_SCORE 99
#define DEFAULT_SCORE 50

// NOTE: accepting ints rather than chars for the symbol below is hacky and was
// done just to avoid having to do casts within
struct morse_table {
  int (*get_entry_count)();
  size_t (*get_max_sequence_length)();
  int (*get_total_score)();
  bool (*contains)(int sym);
  int (*get_score)(int sym);
  void (*inc_score)(int sym);
  void (*dec_score)(int sym);
  const char *(*get_sequence)(int sym);
};

struct morse_table *init_morse_table();
void uninit_morse_table();

#endif
