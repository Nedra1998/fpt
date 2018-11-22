#include "fpt.h"

#include <stdio.h>

int main() {
  G_init_graphics(600, 600);
  /* SetAutoDisplay(0); */
  /* DSetColorRgb(1.0, 1.0, 1.0); */
  /* FillRectangle(50, 50, 100, 100); */
  /* Point(250, 250); */
  /* DSetColorRgb(0.0, 1.0, 1.0); */
  /* Line(-50, -25, 180, 200); */
  /* FillTriangle(150, 150, 250, 150, 200, 350); */
  /* [> SafeLine(-50, -25, 180, 500); <] */
  /* int x[25] = {0, 20, 40, 60, 80, 100, 120, 140, 160, 180, 200, 200, 180, 160, 140, 120, 100, 80, 60, 40, 20, 0}; */
  /* int y[25] = {0, 10, 00, 20, 00, 30, 00, 40, 00, 50, 000, 100, 50, 100, 60, 100, 70, 100, 80, 100, 90, 100}; */
  /* FillPolygon(x, y, 23); */
  /* DSetColorRgb(1.0, 0.0, 0.0); */
  /* Polygon(x, y, 23); */
  /* FillCircle(300, 300, 50); */

  G_rgb(1.0, 1.0, 1.0);
  /* DSetColorRgb(1.0, 1.0, 1.0); */
  while(1){
    double p[2];
    /* IGetClick(p); */
    G_wait_click(p);
    if(p[0] < 100 && p[1] < 100){
      break;
    }
    if(p[0] > 500 && p[1] < 100){
      /* RenderPresent(); */
    }
    G_fill_circle(p[0], p[1], 10);
  }

  G_save_image_to_file("tmp.png");
  G_term_graphics();
  return 0;
}
