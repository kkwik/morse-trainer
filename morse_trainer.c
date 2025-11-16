#include "morse_trainer.h"
#include "morse_player.h"
#include "morse_table.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>

static struct morse_entry **morse_table;
char current_char;

char sanitize_key_input(char ch_in) {
#pragma GCC diagnostic warning "-Wtype-limits"
  if (ch_in < 0 || ch_in >= MORSE_TABLE_BUFFER_SIZE) {
    return 0; // This check might be useless, gcc seems to think so
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

const struct morse_entry **trainer_get_table(const struct morse_entry **buffer,
                                             int buffer_size) {
  if (buffer_size < MORSE_TABLE_BUFFER_SIZE) {
    return NULL;
  }

  for (int i = 0; i < buffer_size; i++) {
    if (morse_table[i]) {
      buffer[i] = morse_table[i];
    }
  }
  return (const struct morse_entry **)buffer;
}

void trainer_start() {
  srand(time(NULL) + 1);
  morse_table = init_morse_table();

  player_setup(get_table_max_code_length());
}

void trainer_stop() {
  uninit_morse_table();
  player_teardown();
}

void trainer_next() {
  // Runs two passes through the morse table to calculate the total score and
  // then make a choice bounded by that total. Definitely more efficient ways to
  // do this
  // TODO: Could add an int to the table and track total_score through that.
  // Would require synchronization anytime score is updated

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

// TODO: the current implementation uses a mutex to prevent multiple threads
// playing at the same time, but this results in a queue of threads forming to
// play. If the user is entering keys without actually listening to the sequence
// this can cause a desync of what is being played vs what the program is
// expecting the user to be guessing for. This is an edge case that shouldn't
// really occur for a legitimate user (because answering without hearing the
// whole sequence is not possible) but really it would be nice for the next
// request to play to cancel the current one
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
    increment_score(ch);
    increment_score(current_char);
  } else {
    decrement_score(ch);
  }

  char tmp = current_char;
  current_char = 0;
  return tmp;
}
