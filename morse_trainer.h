#ifndef MORSE_TRAINER_H
#define MORSE_TRAINER_H
#include <stdbool.h>

#define MORSE_TABLE_LENGTH 128
char sanitize_key_input(char in);
void trainer_start();
void trainer_next();
void trainer_play();
bool trainer_guess(char ch);
void trainer_stop();

#endif
