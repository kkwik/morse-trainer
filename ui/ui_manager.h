#ifndef UI_MANAGER_H
#define UI_MANAGER_H
#include <ncurses.h>
#define UNSET_INT -1
#include "../morse/guess_history.h"
#include "../morse/morse_table.h"
#include "../morse/trainer_config.h"
#include <stdbool.h>

bool ui_setup(const struct morse_table *t, struct guess_history *h,
              struct trainer_config *c);
void ui_handle_mouse_input(MEVENT *event);
void ui_redraw_windows();
void ui_update_content();
void ui_teardown();

#endif
