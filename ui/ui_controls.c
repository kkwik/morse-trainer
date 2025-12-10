#include "ui_controls.h"
#include <ncurses.h>
#include <string.h>

static WINDOW *controls_w = NULL;
static struct trainer_config *config = NULL;

// Things we want it to do:
// WPM, disable/enable groups of symbols i.e. letters, numbers, symbols
void init_ui_controls(struct trainer_config *c) { config = c; }

void ui_controls_handle_mouse_input(MEVENT *event) {
  (void)event;
  return;
}

bool ui_controls_redraw(WINDOW *newWin) {
  delwin(controls_w);
  controls_w = newWin;

  int rows, cols;
  getmaxyx(controls_w, rows, cols);
  (void)rows;

  wborder(controls_w, 0, 0, 0, 0, 0, 0, 0, 0);
  char *controls_title = "Controls";
  mvwprintw(controls_w, 0, (cols / 2) - (strlen(controls_title) / 2), "%s",
            controls_title);

  return true;
}

void ui_controls_update_content() {
  int width, height;
  getmaxyx(controls_w, height, width);
  int border_margin = 1;
  width -= 2 * border_margin;
  height -= 2 * border_margin;

  mvwprintw(controls_w, 1, 1, "+%d-", config->wpm);

  wrefresh(controls_w);
}

void ui_controls_teardown() { delwin(controls_w); }
