#include "morse.h"
#include "morse_player.h"
#include <ncurses.h>
#include <panel.h>
#include <stdbool.h>
#include <string.h>

extern struct morse_symbol g_morse_table[];

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

char sanitize_key_input(char in) {
#pragma GCC diagnostic warning "-Wtype-limits"
  if (in < 0 || in >= MORSE_TABLE_LENGTH) {
    return ' '; // This check might be useless, gcc seems to think so
  }

  if (in > 'a' && in < 'z') {
    return in - 32; // Capitalize
  }

  // We'll assume this is safe (it probably isn't)
  if (g_morse_table[(int)in].code) {
    return in;
  }

  return ' '; // Catch fallthrough
}

int main() {
  player_setup();
  struct player_config config = {.amp = 0.2, .hz = 800, .wpm = 25};
  ui_setup();
  ui_draw();

  while (true) {
    int ch = getch();
    if (ch == KEY_RESIZE) {
      clear();
    }
    char rec = sanitize_key_input(ch);

    ui_draw();

    char msg[20] = "You said: ";
    strncat(msg, &rec, 1);

    int y, x;
    getmaxyx(stdscr, y, x);
    y = y * 0.5;
    x = (x * 0.5) - strlen(msg);
    mvwprintw(stdscr, y, x, "%s", msg);

    refresh();

    if (rec == ' ') {
      continue;
    }
    play_morse_char(config, g_morse_table[(int)rec].code);
  }

  player_teardown();
  return 0;
}
