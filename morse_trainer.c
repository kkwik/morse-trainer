#include "morse_trainer.h"
#include "morse_player.h"
#include "morse_table.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>

struct morse_entry **morse_table;
char current_char;

char sanitize_key_input(char ch_in) {
#pragma GCC diagnostic warning "-Wtype-limits"
  if (ch_in < 0 || ch_in >= MORSE_TABLE_BUFFER_SIZE) {
    return ' '; // This check might be useless, gcc seems to think so
  }

  if (ch_in >= 'a' && ch_in <= 'z') {
    return ch_in - 32; // Capitalize
  }

  if (morse_table[(int)ch_in]) {
    return ch_in;
  }

  return 0; // Catch fallthrough
}

void decrement_score(char ch) {
  if (morse_table[(int)ch]->score > 1) {
    morse_table[(int)ch]->score--;
  }
}

void increment_score(char ch) {
  if (morse_table[(int)ch]->score < MAX_SCORE) {
    morse_table[(int)ch]->score++;
  }
}

int **trainer_stats(int **buffer, int buffer_size) {
  if (buffer_size < MORSE_TABLE_BUFFER_SIZE) {
    return NULL;
  }

  for (int i = 0; i < buffer_size; i++) {
    if (morse_table[i]) {
      buffer[i] = &morse_table[i]->score;
    }
  }
  return buffer;
}

void trainer_start() {
  srand(time(NULL) + 1);
  morse_table = init_morse_table();

  player_setup(table_max_code_length());
}

void trainer_stop() {
  uninit_morse_table();
  player_teardown();
}

void trainer_next() {
  // Runs two passes through the morse table to calculate the total score and
  // then make a choice bounded by that total. Definitely more efficient ways to
  // do this

  current_char = 'F';
  int total_score = 0;
  for (int i = 0; i < MORSE_TABLE_BUFFER_SIZE; i++) {
    if (morse_table[i]) {
      total_score += morse_table[i]->score;
    }
  }

  int choice = rand() % total_score;
  for (int i = 0; i < MORSE_TABLE_BUFFER_SIZE; i++) {
    if (morse_table[i]) {
      choice -= morse_table[i]->score;
      if (choice < 0) {
        current_char = (char)i;
        return;
      }
    }
  }
}

void trainer_play() {
  const char *table_seq = morse_table[(int)current_char]->code;
  char *seq = malloc(strlen(table_seq) * sizeof(char));
  strcpy(seq, table_seq);

  struct player_config *config = malloc(sizeof(struct player_config));
  config->amp = 0.2;
  config->hz = 800;
  config->wpm = 25;
  config->code = seq;

  thrd_t thread;
  thrd_create(&thread, thread_play_morse_char, config);
  thrd_detach(thread);
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
