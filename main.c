#include "morse_trainer.h"
#include <ncurses.h>
#include <ncursesw/ncurses.h>
#include <panel.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

void ui_setup() {
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  curs_set(1);
}

// TODO: rename to something more accurate
void ui_draw() {
  border(0, 0, 0, 0, 0, 0, 0, 0);
  mvwprintw(stdscr, 0, 2, "Morse Trainer");
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
  ui_setup();
  ui_draw();
  refresh();

  // General loop
  while (true) {
    trainer_next();
    trainer_play();

    int ch;
    char guess;
    do {
      ch = getch();

      // Handle special cases/input
      switch (ch) {
      case KEY_RESIZE:
        clear();
        ui_draw();
        refresh();
        continue; // break would allow special characters to be evaluated below
      }

      guess = sanitize_key_input(ch);
    } while (guess == 0);

    char msg[20] = "Your guess: ";
    strncat(msg, &guess, 1);

    int y, x;
    getmaxyx(stdscr, y, x);
    y = y * 0.5;
    x = (x * 0.5) - strlen(msg);
    mvwprintw(stdscr, y, x, "%s", msg);

    refresh();

    char answer = trainer_guess(guess);
    char msg2[20] = "The answer was: ";
    strncat(msg2, &answer, 1);
    mvwprintw(stdscr, y + 1, x, "%s", msg2);
  }

  exit_program(0); // Just here incase I change the logic later
  return 0;
}
