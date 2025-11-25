#ifndef UI_MANAGER_H
#define UI_MANAGER_H
#define UNSET_INT -1
#include "../morse/guess_history.h"
#include "../morse/morse_table.h"
#include <stdbool.h>

// The purpose of this class? will be to orchestrate what windows we have
// Give those windows information on where they are and how big they are
// Call draw on them
// Let them know about size changes
//
// Those windows should be able to communicate the minimum size they require and
// the manager should prioritize available space based on that
bool ui_setup(const struct morse_table *t, struct guess_history *h);
void ui_redraw_all_windows();
void ui_draw_history();
void ui_draw_stats();
void ui_teardown();

#endif
