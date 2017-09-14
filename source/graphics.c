#include "graphics.h"

#include <SDL2/SDL.h>

static SDL_Window* window_ = NULL;
static SDL_Renderer* renderer_ = NULL;
static int window_should_close_ = 0;
static int size_[2] = {0,0};

int InitFPT(unsigned int w, unsigned int h){
  if(SDL_Init(SDL_INIT_VIDEO) < 0){
    printf("SDL could not be initialized! SDL_Error: %s\n", SDL_GetError());
    return 0;
  }else if (SDL_CreateWindowAndRenderer(w, h, SDL_WINDOW_RESIZABLE, &window_, &renderer_)){
    printf("Could not create SDL window and renderer! SDL_Error: %s\n", SDL_GetError());
    return 0;
  }
  RenderPresent(renderer_);
  SDL_GetWindowSize(window_, &size_[0], &size_[1]);
  return 1;
}

int TermFPT(){
  if(renderer_ != NULL){
    SDL_DestroyRenderer(renderer_);
  }
  if(window_ != NULL){
    SDL_DestroyWindow(window_);
  }
  return 1;
}

int ShouldClose(){
  return window_should_close_;
}

int SetShouldClose(int should_close){
  window_should_close_ = should_close;
  return window_should_close_;
}

int GetCurrentSize(int size[2]){
  if(window_!= NULL){
    SDL_GetWindowSize(window_, &size[0], &size[1]);
    size_[0] = size[0];
    size_[1] = size[1];
    return 1;
  }else{
    return 0;
  }
}

int DGetCurrentSize(double size[2]){
  if(window_ != NULL){
    int size_int[2] = {0,0};
    SDL_GetWindowSize(window_, &size_int[0], &size_int[1]);
    size[0] = (double)size_int[0];
    size[1] = (double)size_int[1];
    size_[0] = size[0];
    size_[1] = size[1];
    return 1;
  }else{
    return 0;
  }
}

int RenderPresent(){
  SDL_RenderPresent(renderer_);
}

int ISetColorRgb(int r, int g, int b){
  return SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
}

int DSetColorRgb(double r, double g, double b){
  return SDL_SetRenderDrawColor(renderer_, (int)(255*r), (int)(255*g),(int)(255*b), 255);
}

int Point(int x, int y){
  int ret = SDL_RenderDrawPoint(renderer_, x, size_[1] - 1 - y);
  RenderPresent();
  return ret;
}

int SavePoint(int x, int y){
  int ret = 0;
  int size[2];
  GetCurrentSize(size);
  if(x > 0 && y > 0 && x < size[0] && y < size[1]){
    ret = Point(x, size_[1] - 1 - y);
  }
  RenderPresent();
  return ret;
}

int Line(int x1, int y1, int x2, int y2){
  int ret = 0;
  ret = SDL_RenderDrawLine(renderer_, x1, size_[1] - 1 - y1, x2, size_[1] - 1 - y2);
  RenderPresent();
  return ret;
}

int SafeLine(int x1, int y1, int x2, int y2){
  if(x1 > 0 && y1 > 0 && x2 > 0 && y2 > 0 && x1 < size_[0] && x1 < size_[1] && x2 < size_[0] && y2 < size_[1]){
    return Line(x1, y1, x2, y2);
  }else{
    double xs = x1, ys = y1, xe = x2, ye = y2;
    // Check clipping for y and 0
    if(ys >= 0){
      if(ys < 0){
        double t = (0-ys)/(ye-ys);
        xe = xs + t*(xe-xs);
        ye=0;
      }
    }else{
      if(ys>=0){
        double t = (0-ys)/(ye-ys);
        xs=xs+t*(xe-xs);
        ys=0;
      }else{
        return 0;
      }
    }
    // Check clipping for y and height/size_[1]
    if(ys < size_[1]){
      if(ye>=size_[1]){
        double t = (size_[1] - 1 - ys)/(ye-ys);
        xe = xs + t*(xe-xs);
        ye = size_[1] - 1;
      }
    }else{
      if(ye< size_[1]){
        double t = (size_[1] - 1 - ys)/(ye-ys);
        xs = xs + t*(xe-xs);
        ys = size_[1] - 1;
      }else{
        return 0;
      }
    }
    // Check clipping for x and 0
    if(xs >= 0){
      if(xe < 0){
        double t = (0 - xs)/(xe-xs);
        ye = ys + t*(ye-ys);
        xe = 0;
      }
    }else{
      if(xe >= 0){
        double t = (0- xs)/(xe-xs);
        ys = ys + t * (ye-ys);
        xs = 0;
      }
      else{
        return 0;
      }
    }
    // Check clipping for x and width/size_[0]
    if(xs < size_[0]){
      if(xe >= size_[0]){
        double t = (size_[0] - 1 - xs)/(xe-xs);
        ye = ys + t*(ye-ys);
        xe = size_[0] - 1;
      }
    }else{
      if (xe <size_[0]){
        double t = (size_[0] - 1 -xs)/(xe-xs);
        ys = ys + t*(ye-ys);
        xs = size_[0]  -1;
      }
    }
    x1 = (int)xs;
    y1 = (int)ys;
    x2 = (int)xe;
    y2 = (int)ye;

    return Line(x1, y1, x2, y2);
  }
}

int Rectangle(int x, int y, int w, int h){
  int ret = 0;
  SDL_Rect rect = {x, size_[1] - 1- y - h, w, h};
  ret = SDL_RenderDrawRect(renderer_, &rect);
  RenderPresent();
  return ret;
}

int FillRectangle(int x, int y, int w, int h){
  int ret = 0;
  SDL_Rect rect = {x, size_[1] - 1 - y - h, w, h};
  ret = SDL_RenderFillRect(renderer_, &rect);
  RenderPresent();
  return ret;
}
