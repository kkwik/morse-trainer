#include "guess_history.h"
#include "morse_table.h"
#include "morse_trainer.h"
#include <assert.h>
#include <locale.h>
#include <ncurses.h>
#include <ncursesw/ncurses.h>
#include <panel.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static const struct morse_entry **morse_table;
static struct guess_history history;

// NCURSES Windows
WINDOW *history_w;
int history_min_width = 0;
char *history_msg_template = "Played %c [%s], you answered %c [%s]";
WINDOW *main_w;
WINDOW *stats_w;

char *progress_indicator[9] = {" ",
                               "\xE2\x96\x8F",
                               "\xE2\x96\x8E",
                               "\xE2\x96\x8D",
                               "\xE2\x96\x8C",
                               "\xE2\x96\x8B",
                               "\xE2\x96\x8A",
                               "\xE2\x96\x89",
                               "\xE2\x96\x88"};

void compute_history_min_width() {
  history_min_width = 0;
  bool on_percent = false;

  for (size_t i = 0; i < strlen(history_msg_template); i++) {
    if (history_msg_template[i] == '%') {
      on_percent = true;
      history_min_width++;
    } else if (on_percent) {
      switch (history_msg_template[i]) {
      case 'c':
        break; // %c resolves to 1 char and we've already counted the %
      case 's':
        history_min_width += get_table_max_code_length() - 1;
        break;
      default:
        history_min_width++; // Not a % case we handle, assume literal %
        break;
      }
    } else {
      history_min_width++; // Normal char
    }
  }
}

void ui_setup() {
  setlocale(LC_ALL, "");
  initscr();
  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  curs_set(1);
}

void ui_draw_history() {
  int width, height;
  getmaxyx(history_w, height, width);
  int border_margin = 1;
  width -= 2 * border_margin;
  height -= 2 * border_margin;

  if (width < history_min_width) {
    wrefresh(history_w);
    return;
  }

  for (int i = 0; i < history.capacity; i++) {
    if (height - i <= 0) {
      // We've reached the top of the window, cannot draw more
      break;
    }

    struct guess_entry entry;
    if (history_get_entry(&entry, &history, i)) {
      mvwprintw(history_w, height - i, border_margin,
                "Played %c [%s], you answered %c [%s]", entry.answer,
                morse_table[(int)entry.answer]->code, entry.guess,
                morse_table[(int)entry.guess]->code);

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
  wrefresh(history_w);
}

void ui_draw_main() {}

void ui_draw_stats() {
  int width, height;
  getmaxyx(history_w, height, width);
  int border_margin = 1;
  width -= 2 * border_margin;
  height -= 2 * border_margin;

  int stat_blocks = 11; // This is derived from MAX_SCORE / 9 progress segments

  int code_count = 0; // Since code locations are not contiguous we need to
                      // track which code we are on separately from i
  for (int i = 0; i < MORSE_TABLE_BUFFER_SIZE; i++) {
    if (!morse_table[i]) {
      continue;
    }

    mvwprintw(stats_w, border_margin + code_count, border_margin, "%c ",
              (char)i);
    int entry_score = morse_table[i]->score;
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

void ui_redraw_all_windows() {
  // NOTE: Turning formatting off to make repeated code more coherent
  /* clang-format off */
  clear();
  wborder(stdscr, 0, 0, 0, 0, 0, 0, 0, 0);
  mvwprintw(stdscr, 0, 2, "Morse Trainer");
  wrefresh(stdscr);

  int avail_cols = COLS - 2;
  int avail_rows = LINES - 2;
  int border_margin = 1;
  int sub_col_width = avail_cols / 3;

  // Cleanup Windows
  delwin(history_w);
  delwin(main_w);
  delwin(stats_w);

  // Create Windows
  history_w =     newwin(avail_rows, sub_col_width, 0 + border_margin, border_margin + (0 * sub_col_width));
  main_w =        newwin(avail_rows, sub_col_width, 0 + border_margin, border_margin + (1 * sub_col_width));
  stats_w = newwin(avail_rows, sub_col_width, 0 + border_margin, border_margin + (2 * sub_col_width));

  // Draw Borders
  wborder(history_w,     0, 0, 0, 0, 0, 0, 0, 0);
  wborder(main_w,        0, 0, 0, 0, 0, 0, 0, 0);
  wborder(stats_w, 0, 0, 0, 0, 0, 0, 0, 0);

  // Draw Titles
  char *history_title =     "Past Guesses";
  char *main_title =        "Main Window";
  char *proficiency_title = "Proficiency";
  mvwprintw(history_w,     0, (sub_col_width / 2) - (strlen(history_title)     / 2), "%s", history_title);
  mvwprintw(main_w,        0, (sub_col_width / 2) - (strlen(main_title)        / 2), "%s", main_title);
  mvwprintw(stats_w,       0, (sub_col_width / 2) - (strlen(proficiency_title) / 2), "%s", proficiency_title);

  // Draw Window Content
  ui_draw_history();
  ui_draw_main();
  ui_draw_stats();
  /* clang-format on */
}

// TODO: rename to something more accurate
void ui_draw() {
  ui_draw_stats();
  ui_draw_history();
  refresh();
}

void ui_teardown() { endwin(); }

void exit_program(int sig) {
  (void)sig;
  trainer_stop();
  ui_teardown();
  exit(0);
}

int main() {
  signal(SIGINT, exit_program);

  trainer_start();
  const struct morse_entry *buffer[MORSE_TABLE_BUFFER_SIZE] = {NULL};
  morse_table = trainer_get_table(buffer, MORSE_TABLE_BUFFER_SIZE);
  if (morse_table == NULL) {
    printf("Could not allocate morse table");
    return -1;
  }

  compute_history_min_width();
  history = *init_history(&history, 100);

  ui_setup();
  ui_redraw_all_windows();

  // General loop
  while (true) {
    trainer_next();
    trainer_play();

    int ch = 0;
    char guess = 0;
    do {
      ch = getch();
      guess = sanitize_key_input(ch);

      // Handle special cases/input
      switch (ch) {
      case KEY_RESIZE:
        ui_redraw_all_windows();
        continue;
      }
    } while (guess == 0);

    char answer = trainer_guess(guess);
    history_add(&history, guess, answer);

    ui_draw_history();
    ui_draw_stats();
  }

  exit_program(0); // Just here incase I change the logic later
  return 0;
}
