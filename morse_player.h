#ifndef MORSE_PLAYER_H
#define MORSE_PLAYER_H

#include <stdbool.h>
#include <stddef.h>

struct player_config {
  double amp;
  int hz;
  int wpm;
};

bool player_setup(size_t max_code_length);
void play_morse_char(struct player_config config, char *code);
bool player_teardown();
#endif
