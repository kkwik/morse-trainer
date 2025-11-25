#include "guess_history.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>

struct guess_history *init_history(struct guess_history *history,
                                   int capacity) {
  // Size and capacity are guess based
  // Latest is based on actual lengths
  history = calloc(1, sizeof(struct guess_history));
  history->buffer = calloc(2 * capacity, sizeof(char));
  history->capacity = capacity;
  history->size = 0;
  history->latest = 0;

  return history;
}

void uninit_history(struct guess_history *history) {
  free(history->buffer);
  free(history);
}

void history_add(struct guess_history *history, char guess, char answer) {
  // TODO: checking size != 0 feels a bit hacky
  // There is an issue where if we increment latest and then set it in place we
  // have abandoned i=0, but it is convenient for access because latest[0] is
  // the latest. If we set and then increment we take care of i=0 but accesses
  // are rougher
  if (history->size != 0) {
    history->latest += 2;
    if (history->latest > 2 * history->capacity) {
      history->latest = 0; // Wrap to beginning
    }
  }

  int index = history->latest;
  history->buffer[index] = guess;
  history->buffer[index + 1] = answer;

  if (history->size < history->capacity) {
    history->size++;
  }
}

bool history_get_entry(struct guess_entry *entry, struct guess_history *history,
                       int rel_index) {
  if (history->size == 0) {
    return false;
  }

  int abs_index = history->latest - (2 * rel_index);
  if (abs_index < 0) {
    if (history->size < history->capacity) {
      // We've attempted to wrap around to the end but we haven't
      // allocated enough for there to be anything there yet

      return false;
    }

    abs_index += 2 * history->capacity; // Wrap around to end
  }

  entry->guess = history->buffer[abs_index];
  entry->answer = history->buffer[abs_index + 1];

  return true;
}
