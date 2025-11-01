#ifndef MORSE_PLAYER_H
#define MORSE_PLAYER_H

#include <stdbool.h>
#include <stddef.h>

struct player_config {
  double amp;
  int hz;
  int wpm;
  char *code;
};

bool player_setup(size_t max_code_length);
void play_morse_char(struct player_config *config);
int thread_play_morse_char(void *);
bool player_teardown();
#endif
