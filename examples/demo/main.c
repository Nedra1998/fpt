#include <stdlib.h>

#include "fpt/fpt.h"

float frand() { return (float)rand() / RAND_MAX; }

int main(int argc, char* argv[]) {
  size_t width = 400, height = 600;
  double x[100], y[100];
  size_t n = 0;
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
  G_rgb(0.0, 1.0, 1.0);
  G_circle(300, 300, 200);
  G_fill_circle(200, 200, 15);
  int key = ' ';
  while (key != 'q') {
    double p[2];
    key = G_wait_event(p);
    printf("KEY: %d\n", key);
    if (key == 'r') {
      G_rgb(frand(), frand(), frand());
    } else if (key == 'd') {
      G_polygon(x, y, n);
    } else if (key == 'f') {
      G_fill_polygon(x, y, n);
    } else if (key == 'c') {
      n = 0;
    } else if (key < 0) {
      x[n] = p[0];
      y[n] = p[1];
      n++;
      G_fill_circle(p[0], p[1], 3);
    }
  }
  G_save_image_to_file("demo.bmp");
  return 0;
}
