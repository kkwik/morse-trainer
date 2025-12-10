#include "ui_stats.h"
#include <string.h>

static const struct morse_table *table = NULL;

static WINDOW *stats_w = NULL;
static int stats_min_width;
static char *progress_indicator[9] = {" ",
                                      "\xE2\x96\x8F",
                                      "\xE2\x96\x8E",
                                      "\xE2\x96\x8D",
                                      "\xE2\x96\x8C",
                                      "\xE2\x96\x8B",
                                      "\xE2\x96\x8A",
                                      "\xE2\x96\x89",
                                      "\xE2\x96\x88"};

void init_ui_stats(const struct morse_table *t) {
  table = t;
  stats_min_width = 9; // TODO: Magic!!
}

void ui_stats_handle_mouse_input(MEVENT *event) {
  (void)event;
  return;
}

bool ui_stats_redraw(WINDOW *newWin) {
  delwin(stats_w);
  stats_w = newWin;

  int rows, cols;
  getmaxyx(stats_w, rows, cols);
  (void)rows;

  wborder(stats_w, 0, 0, 0, 0, 0, 0, 0, 0);
  char *proficiency_title = "Proficiency";
  mvwprintw(stats_w, 0, (cols / 2) - (strlen(proficiency_title) / 2), "%s",
            proficiency_title);

  return true;
}

void ui_stats_update_content() {
  int width, height;
  getmaxyx(stats_w, height, width);
  int border_margin = 1;
  width -= 2 * border_margin;
  height -= 2 * border_margin;

  int stat_blocks = 11; // This is derived from MAX_SCORE / 9 progress segments

  int code_count = 0; // Since code locations are not contiguous we need to
                      // track which code we are on separately from i
  for (int i = 0; i < MORSE_TABLE_BUFFER_SIZE; i++) {
    if (!(table->contains(i))) {
      continue;
    }

    mvwprintw(stats_w, border_margin + code_count, border_margin, "%c ",
              (char)i);
    int entry_score = table->get_score(i);
    int complete_blocks = entry_score / 9;
    int in_progress_block = entry_score % 9;

    for (int drawn_blocks = 0; drawn_blocks < stat_blocks; drawn_blocks++) {
      if (complete_blocks-- > 0) {
        wprintw(stats_w, "%s", progress_indicator[8]);
      } else {
        wprintw(stats_w, "%s", progress_indicator[in_progress_block]);
        in_progress_block = 0; // Set to zero so any remaining blocks are empty
      }
    }

    code_count++;
  }
  wrefresh(stats_w);
}

void ui_stats_teardown() { delwin(stats_w); }
