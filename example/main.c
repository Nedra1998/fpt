#include "FPT.h"

#include <unistd.h>

int main(int argc, char *argv[])
{
  unsigned major, minor;
  G_version(&major, &minor);
  printf("V%u.%u", major, minor);
  G_init_graphics(500, 500);
  G_hex(0xff00ff);
  /* sleep(1); */
  for(int i = 0; i < 50; ++i){
    int x = rand() % 500;
    int y = rand() % 500;
    int r = rand() % 10;
    if(!G_fill_circle(x, y, r)){
      printf("ERR\n");
    }
  }
  G_display_image();
  G_save_image_to_file("test.xwd");
  sleep(2);
  G_term_graphics();
  return 0;
}
