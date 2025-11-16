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

struct guess_history *init_history(struct guess_history *, int capacity);
void uninit_history(struct guess_history *);
void history_add(struct guess_history *, char, char);
bool history_get_entry(struct guess_entry *, struct guess_history *, int);

#endif
