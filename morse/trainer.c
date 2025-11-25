#include "trainer.h"
#include "audio_player.h"
#include "morse_table.h"
#include <stdatomic.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>

static const struct morse_table *table;
char current_char;
atomic_bool continue_playing_audio = false;
mtx_t audio_subthread_mtx;

char sanitize_key_input(char ch_in) {
#pragma GCC diagnostic warning "-Wtype-limits"
  // NOLINTNEXTLINE
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

void mark_right(char ch) { table->inc_score(ch); }

void mark_wrong(char ch) { table->dec_score(ch); }

void trainer_start(const struct morse_table *t) {
  mtx_init(&audio_subthread_mtx, mtx_plain);
  table = t;
  srand(time(NULL) + 1);

  player_setup(table->get_max_sequence_length());
}

void trainer_stop() {
  uninit_morse_table();
  player_teardown();
  mtx_destroy(&audio_subthread_mtx);
}

void trainer_next() {
  current_char = 'F';

  // Inaccuracy = MAX_SCORE - score, the distance from perfect
  // This contortionism came about because initially lower scores were better
  // but I thought for displaying a higher score being better would be more
  // self-explanatory
  int max_total_score = MAX_SCORE * table->get_entry_count();
  int total_inaccuracy = max_total_score - table->get_total_score();
  int choice = rand() % total_inaccuracy;
  for (int i = 0; i < MORSE_TABLE_BUFFER_SIZE; i++) {
    if (table->contains(i)) {
      choice -= (MAX_SCORE - table->get_score(i));
      if (choice <= 0) {
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

  // Signal any previous thread to stop playing audio
  // Existing thread will cleanup and then release the mutex allowing the new
  // thread to begin
  thrd_t thread;
  continue_playing_audio = false;

  mtx_lock(&audio_subthread_mtx);
  continue_playing_audio = true;
  thrd_create(&thread, thread_play_morse_char, config);
  thrd_detach(thread);
  mtx_unlock(&audio_subthread_mtx);
}

char trainer_guess(char ch) {
  ch = sanitize_key_input(ch);
  if (ch == 0) {
    return false;
  }

  if (current_char == ch) {
    mark_right(ch);
  } else {
    mark_wrong(ch);
    mark_wrong(current_char);
  }

  char tmp = current_char;
  current_char = 0;
  return tmp;
}
