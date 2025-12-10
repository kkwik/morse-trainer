#include "ui_controls.h"
#include "coords.h"
#include <ncurses.h>
#include <string.h>

static WINDOW *controls_w = NULL;
static struct trainer_config *config = NULL;

// NOTE: points relative to stdscr
static struct point speed_up = {};
static struct point speed_down = {};

// Things we want it to do:
// WPM, disable/enable groups of symbols i.e. letters, numbers, symbols
void init_ui_controls(struct trainer_config *c) { config = c; }

void ui_controls_handle_mouse_input(MEVENT *event) {
  struct point event_loc = (struct point){.x = event->x, .y = event->y};
  bool content_change = false;

  if (point_equal(event_loc, speed_up)) {
    config->wpm++;
    content_change = true;
  } else if (point_equal(event_loc, speed_down)) {
    config->wpm--;
    content_change = true;
  }

  if (content_change) {
    ui_controls_update_content();
  }
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

  // WPM Controls
  wmove(controls_w, 1, 1);
  waddstr(controls_w, "WPM: ");
  getyx(controls_w, speed_up.y, speed_up.x); // Store location of + button
  waddstr(controls_w, "+");
  wprintw(controls_w, "%02d", config->wpm);
  getyx(controls_w, speed_down.y, speed_down.x);
  waddstr(controls_w, "-");

  // Because MEVENT is screen relative we must adjust controls_w coords
  int c_x, c_y;
  getbegyx(controls_w, c_y, c_x);
  speed_up.x += c_x;
  speed_up.y += c_y;
  speed_down.x += c_x;
  speed_down.y += c_y;

  wrefresh(controls_w);
}

void ui_controls_teardown() { delwin(controls_w); }
