#ifndef UI_CONTROLS_H
#define UI_CONTROLS_H

#include <ncurses.h>

void init_ui_controls();
bool ui_controls_redraw(WINDOW *newWin);
void ui_controls_update_content();
void ui_controls_teardown();

#endif
