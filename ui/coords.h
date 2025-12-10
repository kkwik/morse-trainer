#ifndef UI_COORDS_H
#define UI_COORDS_H

#include <stdbool.h>

struct point {
  int x;
  int y;
};

struct bounds {
  struct point tl;
  struct point br;
};

bool inside_bounds(struct bounds b, struct point p);
bool point_equal(struct point a, struct point b);

#endif
