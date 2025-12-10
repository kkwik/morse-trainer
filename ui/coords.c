#include "coords.h"

bool inside_bounds(struct bounds b, struct point p) {
  if (b.tl.x < p.x && p.x < b.br.x && b.tl.y < p.y && p.y < b.br.y) {
    return true;
  } else {
    return false;
  }
}

bool point_equal(struct point a, struct point b) {
  return a.x == b.x && a.y == b.y;
}
