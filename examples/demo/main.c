#include <stdlib.h>

#include "fpt/fpt.h"

float frand() { return (float)rand() / RAND_MAX; }

int main(int argc, char* argv[]) {
  size_t width = 400, height = 600;
  G_init_graphics(width, height);
  G_rgb(0.3, 0.3, 0.3);
  G_clear();
  G_wait_key();
  G_set_pen_size(10.0f);
  G_rgb(1.0, 0.0, 0.0);
  G_point(200, 580);
  G_rgb(0.0, 1.0, 0.0);
  G_line(0, 0, width - 1, height - 1);
  G_rgb(0.0, 0.0, 1.0);
  G_rectangle(200, 50, 10, 30);
  G_fill_rectangle(250, 50, 10, 30);
  G_rgb(1.0, 1.0, 0.0);
  G_triangle(10, 300, 40, 30, 60, 250);
  G_fill_triangle(10, 100, 40, 100, 60, 150);
  while (G_wait_key() != 'q') {
    G_rgb(frand(), frand(), frand());
    G_clear();
    /* G_rgb(0.0, 0.0, 0.0); */
    /* G_set_pen_size(10.0f); */
    /* #<{(| for (size_t i = 0; i < 1000; ++i) { |)}># */
    /* G_point(100, 100); */
    /* G_point(0, 0); */
    /* } */
  }
  /* G_save_image_to_file("demo.bmp"); */
  return 0;
}
