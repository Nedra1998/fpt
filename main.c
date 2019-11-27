#include <stdlib.h>
#include <unistd.h>

#include "FPT.c"

int main(int argc, char* argv[]) {
  G_init_graphics(500, 500);
  int key = ' ';
  Gi_rgb(0, 0, 0);
  while (key != 'q') {
    key = G_wait_key();
    for (int i = 0; i < 500; ++i) {
      G_point(rand() % 500, rand() % 500);
    }
  }
  G_close();
  return 0;
}
