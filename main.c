#include "morse/guess_history.h"
#include "morse/morse_table.h"
#include "morse/trainer.h"
#include "morse/trainer_config.h"
#include "ui/ui_manager.h"
#include <assert.h>
#include <ncurses.h>
#include <ncursesw/ncurses.h>
#include <panel.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static const struct morse_table *table_data = NULL;
static struct guess_history *history_data = NULL;
static struct trainer_config *config = NULL;

// TODO: free the allocated stuff above
void exit_program(int sig) {
  (void)sig;
  trainer_stop();
  ui_teardown();
  exit(0);
}

// TODO: Better error handling here to properly dealloc any resources if setup
// fails halfway
bool setup_program() {
  signal(SIGINT, exit_program);

  table_data = init_morse_table();
  if (table_data == NULL) {
    printf("Could not allocate morse table");
    return false;
  }
  trainer_start(table_data);

  history_data = init_history(history_data, 100);
  if (history_data == NULL) {
    printf("Could not allocate guess history");
    return false;
  }

  config = init_trainer_config(config);
  if (config == NULL) {
    printf("Could not allocate configuration");
    return false;
  }

  ui_setup(table_data, history_data, config);

  return true;
}

int main() {
  if (setup_program() != true) {
    return -1;
  }
  ui_redraw_windows();

  // General loop
  while (true) {
    trainer_next();
    trainer_play();

    int ch = 0;
    char guess = 0;
    do {
      ch = getch();

      // Handle special cases/input
      MEVENT event;
      switch (ch) {
      case KEY_RESIZE:
        ui_redraw_windows();
        continue;
      case KEY_MOUSE:
        if (getmouse(&event) == OK) {
          ui_handle_mouse_input(&event);
        }
        continue;
      }

      guess = sanitize_key_input(ch);
    } while (guess == 0);

    char answer = trainer_guess(guess);
    history_add(history_data, guess, answer);

    ui_update_content();
  }

  exit_program(0); // Just here incase I change the logic later
  return 0;
}
