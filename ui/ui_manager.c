#include "ui_manager.h"
#include "ui_controls.h"
#include "ui_history.h"
#include "ui_stats.h"
#include <locale.h>
#include <ncurses.h>
#include <string.h>

static const struct morse_table *table = NULL;
static struct guess_history *history = NULL;

bool ui_setup(const struct morse_table *t, struct guess_history *h,
              struct trainer_config *c) {
  table = t;
  history = h;

  setlocale(LC_ALL, "");
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  curs_set(0);
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

  init_ui_history(t, h);
  init_ui_controls(c);
  init_ui_stats(t);

  return true;
}

void ui_handle_mouse_input(MEVENT *event) {
  (void)event;

  wrefresh(stdscr);
}

void ui_redraw_windows() {
  clear();
  wborder(stdscr, 0, 0, 0, 0, 0, 0, 0, 0);
  mvwprintw(stdscr, 0, 2, "Morse Trainer");
  wrefresh(stdscr);

  int avail_cols = COLS - 2;
  int avail_rows = LINES - 2;
  int border_margin = 1;
  int sub_col_width = avail_cols / 3;

  /* clang-format off */
  ui_history_redraw( newwin(avail_rows, sub_col_width, border_margin, border_margin + (0 * sub_col_width)));
  ui_controls_redraw(newwin(avail_rows, sub_col_width, border_margin, border_margin + (1 * sub_col_width)));
  ui_stats_redraw(   newwin(avail_rows, sub_col_width, border_margin, border_margin + (2 * sub_col_width)));
  /* clang-format on */

  ui_history_update_content();
  ui_controls_update_content();
  ui_stats_update_content();
}

void ui_update_content() {
  ui_history_update_content();
  ui_controls_update_content();
  ui_stats_update_content();
  refresh();
}

void ui_teardown() {
  ui_history_teardown();
  ui_controls_teardown();
  ui_stats_teardown();
  endwin();
}
