#include "coords.h"

bool inside_bounds(struct bounds b, struct point p) {
  if (b.tl.x < p.x && p.x < b.br.x && b.tl.y < p.y && p.y < b.br.y) {
    return true;
  } else {
    return false;
  }
}
