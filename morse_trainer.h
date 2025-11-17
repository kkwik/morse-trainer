#ifndef MORSE_TRAINER_H
#define MORSE_TRAINER_H
#include <stdbool.h>
#include "morse_table.h"

char sanitize_key_input(char in);
void trainer_start(const struct morse_table *);
void trainer_next();
void trainer_play();
char trainer_guess(char ch);
void trainer_stop();

#endif
