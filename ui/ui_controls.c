#include "ui_controls.h"
#include <ncurses.h>
#include <string.h>

static WINDOW *controls_w = NULL;

void init_ui_controls() {}

bool ui_controls_redraw(WINDOW *newWin) {
  delwin(controls_w);
  controls_w = newWin;

  int rows, cols;
  getmaxyx(controls_w, rows, cols);
  (void)rows;

  wborder(controls_w, 0, 0, 0, 0, 0, 0, 0, 0);
  char *main_title = "Main Window";
  mvwprintw(controls_w, 0, (cols / 2) - (strlen(main_title) / 2), "%s",
            main_title);

  return true;
}

void ui_controls_update_content() {}

void ui_controls_teardown() { delwin(controls_w); }
