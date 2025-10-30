#include "morse_trainer.h"
#include <ncurses.h>
#include <panel.h>
#include <stdbool.h>
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

int main() {
  trainer_start();
  ui_setup();
  ui_draw();

  while (true) {
    trainer_next();
    trainer_play();
    int ch = getch();
    if (ch == KEY_RESIZE) {
      clear();
    }
    char guess = sanitize_key_input(ch);
    if (guess == 0) {
      continue;
    }

    ui_draw();

    char msg[20] = "You guessed: ";
    strncat(msg, &guess, 1);

    int y, x;
    getmaxyx(stdscr, y, x);
    y = y * 0.5;
    x = (x * 0.5) - strlen(msg);
    mvwprintw(stdscr, y, x, "%s", msg);

    refresh();

    bool correct = trainer_guess(guess);
		(void)correct;
  }

  trainer_stop();
  return 0;
}
