#include "fpt.h"

#include <unistd.h>

int main(int argc, char* argv[]) {
  G_init_graphics(500, 500);
  G_rgb(1.0, 1.0, 1.0);
  while (1) {
    double p[2];
    G_wait_click(p);
    if (p[0] < 100 && p[1] < 100) {
      break;
    }
    G_fill_circle(p[0], p[1], 50);
  }
  G_term_graphics();
  return 0;
}
