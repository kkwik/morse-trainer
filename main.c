#include "guess_history.h"
#include "morse_table.h"
#include "morse_trainer.h"
#include <assert.h>
#include <ncurses.h>
#include <ncursesw/ncurses.h>
#include <panel.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static const struct morse_entry **morse_table;
static struct guess_history history = {.capacity = 10, .latest = 0};

void ui_setup() {
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  curs_set(1);
}

void ui_draw_history() {
  int bottom = LINES - 2;

  for (int i = 0; i < history.capacity; i++) {

    struct guess_entry entry;
    if (history_get_entry(&entry, &history, i)) {
      mvwprintw(stdscr, bottom - i, 2, "Played %c [%s], you answered %c [%s]",
                entry.answer, morse_table[(int)entry.answer]->code, entry.guess,
                morse_table[(int)entry.guess]->code);
    }
  }
}

void ui_draw_stats() {
  int stat_col_width = 6; // 6 cols to display each stat: i.e. ' A=99 '
  int row_top_padding = 2;
  int col_side_padding = 2;
  int stat_col_count = (COLS - (2 * col_side_padding)) /
                       stat_col_width; // 2 cols on either side for padding

  int code_count = 0; // Since code locations are not contiguous we need to
                      // track which code we are on separately from i
  for (int i = 0; i < MORSE_TABLE_BUFFER_SIZE; i++) {
    if (!morse_table[i]) {
      continue;
    }

    int row = code_count / stat_col_count;
    int col = code_count % stat_col_count;

    int x, y;
    y = row + row_top_padding;
    x = col_side_padding + (col * stat_col_width);

    mvwprintw(stdscr, y, x, " %c=%2d ", (char)i, morse_table[i]->score);
    code_count++;
  }
}

// TODO: rename to something more accurate
void ui_draw() {
  clear();
  border(0, 0, 0, 0, 0, 0, 0, 0);
  mvwprintw(stdscr, 0, 2, "Morse Trainer");
  ui_draw_stats();
  ui_draw_history();
  refresh();
}

void ui_teardown() { endwin(); }

void exit_program(int sig) {
  (void)sig;
  trainer_stop();
  ui_teardown();
  exit(0);
}

int main() {
  signal(SIGINT, exit_program);

  trainer_start();
  const struct morse_entry *buffer[MORSE_TABLE_BUFFER_SIZE] = {NULL};
  morse_table = trainer_get_table(buffer, MORSE_TABLE_BUFFER_SIZE);
  if (morse_table == NULL) {
    printf("Could not allocate morse table");
    return -1;
  }

  history = *init_history(&history, 10);

  ui_setup();
  ui_draw();

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
        ui_draw();
        continue;
      }
    } while (guess == 0);

    char msg[20] = "Your guess: ";
    strncat(msg, &guess, 1);

    int y, x;
    getmaxyx(stdscr, y, x);
    y = y * 0.5;
    x = (x * 0.5) - strlen(msg);
    mvwprintw(stdscr, y, x, "%s", msg);

    char answer = trainer_guess(guess);
    history_add(&history, guess, answer);

    char msg2[20] = "The answer was: ";
    strncat(msg2, &answer, 1);
    mvwprintw(stdscr, y + 1, x, "%s", msg2);

    ui_draw();
  }

  exit_program(0); // Just here incase I change the logic later
  return 0;
}
