#include "ui_history.h"
#include <ncurses.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static const struct morse_table *table = NULL;
static struct guess_history *history = NULL;

static WINDOW *history_w = NULL;
static int history_min_width;
static char *history_msg_template = "Played %c [%s], you answered %c [%s]";

static int compute_history_min_width() {
  int width = 0;
  bool on_percent = false;

  for (size_t i = 0; i < strlen(history_msg_template); i++) {
    if (history_msg_template[i] == '%') {
      on_percent = true;
      width++;
    } else if (on_percent) {
      switch (history_msg_template[i]) {
      case 'c':
        break; // %c resolves to 1 char and we've already counted the %
      case 's':
        width += table->get_max_sequence_length() - 1;
        break;
      default:
        width++; // Not a % case we handle, assume literal %
        break;
      }
    } else {
      width++; // Normal char
    }
  }
  return width;
}

void init_ui_history(const struct morse_table *t, struct guess_history *h) {
  table = t;
  history = h;
  history_min_width = compute_history_min_width();
}

bool ui_history_redraw(WINDOW *newWin) {
  delwin(history_w);
  history_w = newWin;

  int rows, cols;
  getmaxyx(history_w, rows, cols);
  (void)rows;

  wborder(history_w, 0, 0, 0, 0, 0, 0, 0, 0);
  char *history_title = "Past Guesses";
  mvwprintw(history_w, 0, (cols / 2) - (strlen(history_title) / 2), "%s",
            history_title);

  return true;
}

void ui_history_update_content() {
  int width, height;
  getmaxyx(history_w, height, width);
  int border_margin = 1;
  width -= 2 * border_margin;
  height -= 2 * border_margin;

  if (width < history_min_width) {
    // Not enough width to draw history, draw something else
    for (int r = border_margin; r <= height; r++) {
      for (int c = border_margin; c <= width; c++) {
        int i = (r * width) + c;
        char *s[3] = {"\\", "|", "/"};

        mvwprintw(history_w, r, c, "%s", s[i % 3]);
      }
    }
    wrefresh(history_w);
    return;
  } else {
    int surplus_width = width - history_min_width;
    int left_pad = surplus_width / 2;

    for (int i = 0; i < history->capacity; i++) {
      if (height - i <= 0) {
        break; // Top of window reached, cannot draw more
      }

      struct guess_entry entry;
      if (history_get_entry(&entry, history, i)) {
        mvwprintw(history_w, height - i, border_margin + left_pad,
                  "Played %c [%s], you answered %c [%s]", entry.answer,
                  table->get_sequence(entry.answer), entry.guess,
                  table->get_sequence(entry.guess));

        // Clear out any characters from the end of this line to the border
        // Avoids a full wrefresh
        int curs_y, curs_x;
        getyx(history_w, curs_y, curs_x);
        (void)curs_y;
        int line_remaining = width - curs_x;
        for (int l = 0; l <= line_remaining; l++) {
          wprintw(history_w, " ");
        }
      }
    }
  }

  wrefresh(history_w);
}

void ui_history_teardown() { delwin(history_w); }
