#include "trainer_config.h"
#include <stdlib.h>

struct trainer_config *init_trainer_config(struct trainer_config *config) {
  config = malloc(sizeof(struct trainer_config));
  return config;
}
