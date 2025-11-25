#include "guess_history.h"
#include "morse_table.h"
#include "morse_trainer.h"
#include "ui/ui_manager.h"
#include <assert.h>
#include <ncurses.h>
#include <ncursesw/ncurses.h>
#include <panel.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static const struct morse_table *table = NULL;
static struct guess_history *history = NULL;

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

  table = init_morse_table();
  if (table == NULL) {
    printf("Could not allocate morse table");
    return false;
  }
  trainer_start(table);

  history = init_history(history, 100);
  if (history == NULL) {
    printf("Could not allocate guess history");
    return false;
  }

  ui_setup(table, history);

  return true;
}

int main() {
  if (setup_program() != true) {
    return -1;
  }
  ui_redraw_all_windows();

  // General loop
  while (true) {
    trainer_next();
    trainer_play();

    int ch = 0;
    char guess = 0;
    do {
      ch = getch();
      guess = sanitize_key_input(ch);

      // Handle special cases/input
      switch (ch) {
      case KEY_RESIZE:
        ui_redraw_all_windows();
        continue;
      }
    } while (guess == 0);

    char answer = trainer_guess(guess);
    history_add(history, guess, answer);

    ui_draw_history();
    ui_draw_stats();
  }

  exit_program(0); // Just here incase I change the logic later
  return 0;
}
