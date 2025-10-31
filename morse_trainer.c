#include "morse_trainer.h"
#include "morse_player.h"
#include "morse_table.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct morse_entry **g_morse_lookup;

char current_char;

char sanitize_key_input(char ch_in) {
#pragma GCC diagnostic warning "-Wtype-limits"
  if (ch_in < 0 || ch_in >= MORSE_TABLE_LENGTH) {
    return ' '; // This check might be useless, gcc seems to think so
  }

  if (ch_in > 'a' && ch_in < 'z') {
    return ch_in - 32; // Capitalize
  }

  if (g_morse_lookup[(int)ch_in]) {
    return ch_in;
  }

  return 0; // Catch fallthrough
}

void decrement_score(char ch) {
  if (g_morse_lookup[(int)ch]->score > 1) {
    g_morse_lookup[(int)current_char]->score--;
  }
}

void increment_score(char ch) {
  if (g_morse_lookup[(int)ch]->score < MAX_SCORE) {
    g_morse_lookup[(int)current_char]->score++;
  }
}

void trainer_start() {
  srand(time(NULL) + 1);
  g_morse_lookup = init_morse_table();

  player_setup(max_code_length(g_morse_lookup));
}

void trainer_stop() {
  uninit_morse_table(g_morse_lookup);
  player_teardown();
}

void trainer_next() {
  // Runs two passes through the morse table to calculate the total score and
  // then make a choice bounded by that total. Definitely more efficient ways to
  // do this

  current_char = 'F';
  int total_score = 0;
  for (int i = 0; i < MORSE_TABLE_LENGTH; i++) {
    if (g_morse_lookup[i]) {
      total_score += g_morse_lookup[i]->score;
    }
  }

  int choice = rand() % total_score;
  for (int i = 0; i < MORSE_TABLE_LENGTH; i++) {
    if (g_morse_lookup[i]) {
      choice -= g_morse_lookup[i]->score;
      if (choice < 0) {
        current_char = (char)i;
        return;
      }
    }
  }
}

void trainer_play() {
  struct player_config config = {.amp = 0.2, .hz = 800, .wpm = 25};
  play_morse_char(config, g_morse_lookup[(int)current_char]->code);
}

char trainer_guess(char ch) {
  ch = sanitize_key_input(ch);
  if (ch == 0) {
    return false;
  }

  if (current_char == ch) {
    decrement_score(ch);
  } else {
    increment_score(ch);
    increment_score(current_char);
  }

  char tmp = current_char;
  current_char = 0;
  return tmp;
}
