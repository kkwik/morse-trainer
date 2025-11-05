#ifndef MORSE_TRAINER_H
#define MORSE_TRAINER_H
#include <stdbool.h>

char sanitize_key_input(char in);
const struct morse_entry **trainer_get_table(const struct morse_entry **buffer,
                                             int buffer_size);
void trainer_start();
void trainer_next();
void trainer_play();
char trainer_guess(char ch);
void trainer_stop();

#endif
