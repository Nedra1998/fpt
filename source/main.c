#include "fpt.h"

#include <stdio.h>

int main() {
  InitFPT(500, 500);
  DSetColorRgb(1.0, 1.0, 1.0);
  FillRectangle(50, 50, 100, 100);
  /* Point(250, 250); */
  /* Line(-50, -25, 180, 200); */
  /* SafeLine(-50, -25, 180, 500); */
  GetKey();
  TermFPT();
  return 0;
}
