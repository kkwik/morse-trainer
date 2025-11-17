#include "morse_trainer.h"
#include "morse_player.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>

static const struct morse_table *table;
char current_char;

char sanitize_key_input(char ch_in) {
#pragma GCC diagnostic warning "-Wtype-limits"
  if (ch_in < 0 || ch_in >= MORSE_TABLE_BUFFER_SIZE) {
    return 0; // This check might be useless, gcc seems to think so
  }

  if (ch_in >= 'a' && ch_in <= 'z') {
    return ch_in - 32; // Capitalize
  }

  if (table->contains(ch_in)) {
    return ch_in;
  }

  return 0; // Catch fallthrough
}

void decrement_score(char ch) {
  int c_score = table->get_score(ch);
  if (c_score > 1) {
    table->set_score(ch, --c_score);
  }
}

void increment_score(char ch) {
  int c_score = table->get_score(ch);
  if (c_score > MAX_SCORE) {
    table->set_score(ch, ++c_score);
  }
}

void trainer_start(const struct morse_table *t) {
  table = t;
  srand(time(NULL) + 1);

  player_setup(table->get_max_sequence_length());
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
    if (table->contains(i)) {
      total_score += table->get_score(i);
    }
  }

  int choice = rand() % total_score;
  for (int i = 0; i < MORSE_TABLE_BUFFER_SIZE; i++) {
    if (table->contains(i)) {
      choice -= table->get_score(i);
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
  const char *table_seq = table->get_sequence(current_char);
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
