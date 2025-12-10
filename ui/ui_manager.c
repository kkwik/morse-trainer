#include "ui_manager.h"
#include "bounds.h"
#include "ui_controls.h"
#include "ui_history.h"
#include "ui_stats.h"
#include <locale.h>
#include <ncurses.h>
#include <string.h>

static const struct morse_table *table = NULL;
static struct guess_history *history = NULL;

// Bounds for each window, in order HISTORY, CONTROLS, STATS
// Order here has no bearing on layout
static struct bounds window_bounds[3] = {{}, {}, {}};

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

  // Figure out which window the click was in
  if (inside_bounds(window_bounds[0], event->x, event->y)) {
    ui_history_handle_mouse_input(event);
  } else if (inside_bounds(window_bounds[1], event->x, event->y)) {
    ui_controls_handle_mouse_input(event);
  } else if (inside_bounds(window_bounds[2], event->x, event->y)) {
    ui_stats_handle_mouse_input(event);
  }
}

void ui_redraw_windows() {
  clear();
  wborder(stdscr, 0, 0, 0, 0, 0, 0, 0, 0);
  mvwprintw(stdscr, 0, 2, "Morse Trainer");
  wrefresh(stdscr);

  // Shared information
  int avail_cols = COLS - 2;
  int avail_rows = LINES - 2;
  int border_margin = 1;
  int sub_col_width = avail_cols / 3;

  // History window bounds
  int histo_bx = border_margin + (0 * sub_col_width);
  int histo_by = border_margin;
  int histo_mx = histo_bx + sub_col_width;
  int histo_my = histo_by + avail_rows;
  window_bounds[0] = (struct bounds){
      .bx = histo_bx, .by = histo_by, .mx = histo_mx, .my = histo_my};

  // Controls window bounds
  int contr_bx = border_margin + (1 * sub_col_width);
  int contr_by = border_margin;
  int contr_mx = contr_bx + sub_col_width;
  int contr_my = contr_by + avail_rows;
  window_bounds[1] = (struct bounds){
      .bx = contr_bx, .by = contr_by, .mx = contr_mx, .my = contr_my};

  // stats window bounds
  int stats_bx = border_margin + (2 * sub_col_width);
  int stats_by = border_margin;
  int stats_mx = stats_bx + sub_col_width;
  int stats_my = stats_by + avail_rows;
  window_bounds[2] = (struct bounds){
      .bx = stats_bx, .by = stats_by, .mx = stats_mx, .my = stats_my};

  ui_history_redraw(newwin(avail_rows, sub_col_width, histo_by, histo_bx));
  ui_controls_redraw(newwin(avail_rows, sub_col_width, contr_by, contr_bx));
  ui_stats_redraw(newwin(avail_rows, sub_col_width, stats_by, contr_bx));

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
