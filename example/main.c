#include "FPT.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
  unsigned major, minor;
  G_version(&major, &minor);
  printf("V%u.%u", major, minor);
  return 0;
}
