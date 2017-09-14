#include "fpt.h"

#include <stdio.h>

int main(int argc, char const *argv[]) {
#ifdef __linux__
  printf("This is linux");
#endif
#if _WIN32
  printf("This is windows");
#endif
#if __APPLE__
  printf("This is osx");
#endif
#if __unix__
  printf("This is some unix");
#endif
  /* code */
  return 0;
}
