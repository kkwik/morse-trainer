#include "bounds.h"

bool inside_bounds(struct bounds b, int x, int y) {
  if (b.bx < x && x < b.mx && b.by < y && y < b.my) {
    return true;
  } else {
    return false;
  }
}
