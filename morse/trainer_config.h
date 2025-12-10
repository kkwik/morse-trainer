#ifndef MORSE_TRAINER_CONFIG_H
#define MORSE_TRAINER_CONFIG_H

struct trainer_config {
  int wpm;
};

struct trainer_config *init_trainer_config(struct trainer_config *config);

#endif
