#ifndef UI_STATS_H
#define UI_STATS_H

#include "../morse/guess_history.h"
#include "../morse/morse_table.h"
#include <ncurses.h>

void init_ui_stats(const struct morse_table *t);
bool ui_stats_redraw(WINDOW *newWin);
void ui_stats_update_content();
void ui_stats_teardown();

#endif
