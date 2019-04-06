#include "fpt.h"

#include <unistd.h>

int main(int argc, char* argv[]) {
  unsigned major, minor;
  G_version(&major, &minor);
  printf("V%u.%u\n", major, minor);
  G_init_graphics(500, 500);
  G_hex(0xff00ff);
  for (int i = 0; i < 50; ++i) {
    int p[2] = {rand() % 500, rand() % 500};
    int r = rand() % 10;
    if (!G_fill_circle(p[0], p[1], r)) {
      printf("ERR\n");
    }
  }
  G_clear_events();
  G_display_image();
  G_wait_key();
  G_term_graphics();
  return 0;
}
