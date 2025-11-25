#ifndef GUESS_HISTORY_H
#define GUESS_HISTORY_H

#include <stdbool.h>

struct guess_entry {
  char guess;
  char answer;
};

struct guess_history {
  char *buffer;
  int capacity;
  int size;
  int latest;
};

struct guess_history *init_history(struct guess_history *history, int capacity);
void uninit_history(struct guess_history *history);
void history_add(struct guess_history *history, char guess, char answer);
bool history_get_entry(struct guess_entry *entry, struct guess_history *history,
                       int rel_index);

#endif
