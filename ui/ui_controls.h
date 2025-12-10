#ifndef UI_CONTROLS_H
#define UI_CONTROLS_H

#include "../morse/trainer_config.h"
#include <ncurses.h>

void init_ui_controls(struct trainer_config *c);
void ui_controls_handle_mouse_input(MEVENT *event);
bool ui_controls_redraw(WINDOW *newWin);
void ui_controls_update_content();
void ui_controls_teardown();

#endif
