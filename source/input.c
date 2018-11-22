#include "input.h"

#include <SDL2/SDL.h>

#include "graphics.h"

int Gi_wait_click(int p[2]){
  SDL_Event event;
  while(event.type != SDL_MOUSEBUTTONDOWN){
    SDL_WaitEvent(&event);
  }
  int size[2];
  GetCurrentSize(size);
  p[0] = event.button.x;
  p[1] = size[1] - event.button.y;
  return 1;
}

int G_wait_click(double p[2]){
  int p_int[2];
  int ret = Gi_wait_click(p_int);
  p[0] = (double)p_int[0];
  p[1] = (double)p_int[1];
  return ret;
}

int G_wait_key(){
  SDL_Event event;
  while(event.type != SDL_KEYDOWN){
    SDL_WaitEvent(&event);
  }
  return event.key.keysym.sym;
}

int GetInt(){
  int v;
  scanf("%i", &v);
  return v;
}

void PrintInt(int v){
  printf("%i", v);
}

double GetDouble(){
  double v;
  scanf("%lf", &v);
  return v;
}

void PrintDouble(double v){
  printf("%lf", v);
}

void GetString(const char* v){
  scanf("%s", v);
}

void PrintString(const char* v){
  printf("%s", v);
}
