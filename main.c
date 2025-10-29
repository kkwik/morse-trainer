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

void ui_draw() {
  border(0, 0, 0, 0, 0, 0, 0, 0);
  mvwprintw(stdscr, 0, 2, "Morse Trainer");
}

void ui_teardown() { endwin(); }

int main() {
  ui_setup();
  ui_draw();

  while (true) {
    int ch = getch();
    if (ch == KEY_RESIZE) {
      clear();
    }

    ui_draw();

    char msg[20] = "You said: ";
    strncat(msg, (char *)&ch, 1);

    int y, x;
    getmaxyx(stdscr, y, x);
    y = y * 0.5;
    x = (x * 0.5) - strlen(msg);
    mvwprintw(stdscr, y, x, "%s", msg);

    refresh();
  }

  return 0;
}
