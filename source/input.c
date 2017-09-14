#include "input.h"

#include <SDL2/SDL.h>

int IGetClick(int p[2]){
  return 1;
}

int DGetClick(double p[2]){
  return 1;
}

int GetKey(){
  int ret = 1;
  SDL_Event event;
  while(event.type != SDL_KEYDOWN){
    ret = SDL_WaitEvent(&event);
  }
  ret = event.key.keysym.sym;
  return ret;
}

int GetInt(){

}
