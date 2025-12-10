#include "ui_manager.h"
#include "coords.h"
#include "ui_controls.h"
#include "ui_history.h"
#include "ui_stats.h"
#include <locale.h>
#include <ncurses.h>
#include <string.h>

static const struct morse_table *table = NULL;
static struct guess_history *history = NULL;

static struct bounds history_bounds = {};
static struct bounds controls_bounds = {};
static struct bounds stats_bounds = {};

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
  struct point event_loc = (struct point){.x = event->x, .y = event->y};
  if (inside_bounds(history_bounds, event_loc)) {
    ui_history_handle_mouse_input(event);
  } else if (inside_bounds(controls_bounds, event_loc)) {
    ui_controls_handle_mouse_input(event);
  } else if (inside_bounds(stats_bounds, event_loc)) {
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

  /* clang-format off */
  // History window bounds
  struct point histo_tl = (struct point){.x = border_margin + (0 * sub_col_width),
	                                 .y = border_margin};
  struct point histo_br = (struct point){.x = histo_tl.x + sub_col_width,
                                         .y = histo_tl.y + avail_rows};
  history_bounds = (struct bounds){.tl = histo_tl, .br = histo_br};

  // Controls window bounds
  struct point contr_tl = (struct point){.x = border_margin + (1 * sub_col_width),
                                         .y = border_margin};
  struct point contr_br = (struct point){.x = contr_tl.x + sub_col_width,
                                         .y = contr_tl.y + avail_rows};
  controls_bounds = (struct bounds){.tl = contr_tl, .br = contr_br};

  // Stats window bounds
  struct point stats_tl = (struct point){.x = border_margin + (2 * sub_col_width),
                                         .y = border_margin};
  struct point stats_br = (struct point){.x = stats_tl.x + sub_col_width,
                                         .y = stats_tl.y + avail_rows};
  stats_bounds = (struct bounds){.tl = stats_tl, .br = stats_br};
  /* clang-format off */

  ui_history_redraw(newwin(avail_rows, sub_col_width, histo_tl.y, histo_tl.x));
  ui_controls_redraw(newwin(avail_rows, sub_col_width, contr_tl.y, contr_tl.x));
  ui_stats_redraw(newwin(avail_rows, sub_col_width, stats_tl.y, stats_tl.x));

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
