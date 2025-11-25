#ifndef UI_HISTORY_H
#define UI_HISTORY_H

#include "../morse/guess_history.h"
#include "../morse/morse_table.h"
#include <ncurses.h>

void init_ui_history(const struct morse_table *t, struct guess_history *h);
bool ui_history_redraw(WINDOW *newWin);
void ui_history_update_content();
void ui_history_teardown();

#endif
