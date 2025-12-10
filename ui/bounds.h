#ifndef UI_BOUNDS_H
#define UI_BOUNDS_H

#include <stdbool.h>

struct bounds {
  int bx, by, mx, my;
};

bool inside_bounds(struct bounds b, int x, int y);

#endif
