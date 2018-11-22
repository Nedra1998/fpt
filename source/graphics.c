#include "graphics.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <math.h>

#include "input.h"

static SDL_Window* window_ = NULL;
static SDL_Renderer* renderer_ = NULL;
static SDL_Texture* display_ = NULL;
static TTF_Font* font_ = NULL;
static int window_should_close_ = 0;
static int size_[2] = {0,0};

static int font_pt_ = 12;
static int auto_display_ = 1;

void SetAutoDisplay(int val){
  auto_display_ = val;
}

int G_init_graphics(unsigned int w, unsigned int h){
  if(SDL_Init(SDL_INIT_VIDEO) < 0){
    printf("SDL could not be initialized! SDL_Error: %s\n", SDL_GetError());
    return 0;
  }
  if(TTF_Init() == -1){
    printf("SDL TTF could not be initialized! TTF_Error: %s\n", TTF_GetError());
    return 0;
  }
  int flags = IMG_INIT_PNG|IMG_INIT_JPG;
  int initted=IMG_Init(flags);
  if(initted&flags != flags){
    printf("SDL Image could not be initialized! IMG_Error: %s\n", IMG_GetError());
    return 0;
  }
  window_ = SDL_CreateWindow("FPT", 0, 0, w, h, 0);
  if (window_ == NULL){
    printf("Could not create SDL window! SDL_Error: %s\n", SDL_GetError());
    return 0;
  }
  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
  if (renderer_ == NULL){
    printf("Could not create SDL renderer! SDL_Error: %s\n", SDL_GetError());
    return 0;
  }
  SDL_SetRenderDrawBlendMode(renderer_,SDL_BLENDMODE_BLEND);
  display_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888,SDL_TEXTUREACCESS_TARGET,w, h);
  SDL_SetRenderTarget(renderer_, display_);
  SDL_SetRenderDrawColor(renderer_, 0,0,0,255);
  SDL_RenderClear(renderer_);
  RenderPresent(renderer_);
  SDL_GetWindowSize(window_, &size_[0], &size_[1]);
  return 1;
}

int G_term_graphics(){
  if(renderer_ != NULL){
    SDL_DestroyRenderer(renderer_);
  }
  if(window_ != NULL){
    SDL_DestroyWindow(window_);
  }
  if(font_ != NULL){
    TTF_CloseFont(font_);
  }
  IMG_Quit();
  TTF_Quit();
  SDL_Quit();
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
  SDL_SetRenderTarget(renderer_, NULL);
  SDL_RenderCopy(renderer_, display_, NULL, NULL);
  SDL_RenderPresent(renderer_);
  SDL_SetRenderTarget(renderer_, display_);
  return 1;
}

int Gi_rgb(int r, int g, int b){
  return SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
}

int G_rgb(double r, double g, double b){
  return SDL_SetRenderDrawColor(renderer_, (int)(255*r), (int)(255*g),(int)(255*b), 255);
}

int G_point(int x, int y){
  int ret = SDL_RenderDrawPoint(renderer_, x, size_[1] - 1 - y);
  if(auto_display_ == 1){
    RenderPresent();
  }
  return ret;
}

int SavePoint(int x, int y){
  int ret = 0;
  int size[2];
  GetCurrentSize(size);
  if(x > 0 && y > 0 && x < size[0] && y < size[1]){
    ret = G_point(x, size_[1] - 1 - y);
  }
  if(auto_display_ == 1){
    RenderPresent();
  }
  return ret;
}

int G_line(int x1, int y1, int x2, int y2){
  int ret = 0;
  ret = SDL_RenderDrawLine(renderer_, x1, size_[1] - 1 - y1, x2, size_[1] - 1 - y2);
  if(auto_display_ == 1){
    RenderPresent();
  }
  return ret;
}

int SafeLine(int x1, int y1, int x2, int y2){
  if(x1 > 0 && y1 > 0 && x2 > 0 && y2 > 0 && x1 < size_[0] && x1 < size_[1] && x2 < size_[0] && y2 < size_[1]){
    return G_line(x1, y1, x2, y2);
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

    return G_line(x1, y1, x2, y2);
  }
}

int G_rectangle(int x, int y, int w, int h){
  int ret = 0;
  SDL_Rect rect = {x, size_[1] - 1- y - h, w, h};
  ret = SDL_RenderDrawRect(renderer_, &rect);
  if(auto_display_ == 1){
    RenderPresent();
  }
  return ret;
}

int G_fill_rectangle(int x, int y, int w, int h){
  int ret = 0;
  SDL_Rect rect = {x, size_[1] - 1 - y - h, w, h};
  ret = SDL_RenderFillRect(renderer_, &rect);
  if(auto_display_ == 1){
    RenderPresent();
  }
  return ret;
}

int G_triangle(int x1, int y1, int x2, int y2, int x3, int y3){
  int x[3] = {x1,x2,x3};
  int y[3] = {y1,y2,y3};
  return Gi_polygon(x, y, 3);
}

int G_fill_triangle(int x1, int y1, int x2, int y2, int x3,int y3){
  int x[3] = {x1,x2,x3};
  int y[3] = {y1,y2,y3};
  return Gi_fill_polygon(x,y,3);
}

int Gi_polygon(int *x, int *y, int n){
  int i;
  for(i = 0; i < n; i++){
    int j = (i+1)%n;
    SDL_RenderDrawLine(renderer_,  x[i], size_[1] - 1 - y[i], x[j], size_[1] - 1 - y[j]);
  }
  if(auto_display_ == 1){
    RenderPresent();
  }
  return 1;
}

int G_polygon(double *x, double *y, int n){
  int i = 0;
  int *x_int;
  int *y_int;
  for(i = 0; i < n; i++){
    x_int[i] = (int)x[i];
    y_int[i] = (int)y[i];
  }
  return Gi_polygon(x_int, y_int, n);
}

int Gi_fill_polygon(int *x, int* y, int n){
  int i;
  double min = size_[1], max = 0;
  for(i = 0; i < n; i++){
    if (y[i] < min){
      min = y[i];
    }
    if(y[i] > max){
      max = y[i];
    }
  }

  double row;
  for(row = min + 0.01; row < max; row+=1){
    double inter[n];
    int index = 0;
    for(i = 0; i < n;i++){
      int j = (i+1)%n;
      if ((y[i] <= row && y[j] >= row) || (y[i] >= row && y[j] <= row)){
        inter[index] = (row - (double)y[i])*(((double)x[i] - (double)x[j])/((double)y[i] - (double)y[j])) + (double)x[i];
        index++;
      }
    }
    for(i = 0; i < index; i++){
      int j;
      for(j = 0; j < index; j++){
        if(inter[j] > inter[i]){
          double tmp = inter[i];
          inter[i] = inter[j];
          inter[j] = tmp;
        }
      }
    }
    for(i = 0; i < index - 1; i += 2){
      SDL_RenderDrawLine(renderer_, (int)inter[i], size_[1] - 1 - (int)row, (int)inter[i+1], size_[1] - 1 - (int)row);
    }
  }
  if(auto_display_ == 1){
    RenderPresent();
  }

  return 1;
}

int G_fill_polygon(double *x, double *y, int n){
  int *x_int;
  int *y_int;
  int i;
  for(i = 0; i < n; i++){
    x_int[i] = (int)x[i];
    y_int[i] = (int)y[i];
  }
  return Gi_fill_polygon(x_int, y_int, n);
}

int G_circle(int x, int y, int r){
  double theta = 0, dtheta = (3.1415/50.0);
  int x_array[500];
  int y_array[500];
  int n = 0;
  for(theta = 0.0; theta < (3.1415 * 2.0); theta += dtheta){
    x_array[n] = (r * cos(theta)) + x;
    y_array[n] = (r * sin(theta)) + y;
    n++;
  }
  return Gi_polygon(x_array, y_array, n);
}

int G_fill_circle(int x, int y, int r){
  double theta = 0, dtheta = (3.1415/50.0);
  int x_array[100];
  int y_array[100];
  int n = 0;
  for(theta = 0.0; theta < (3.1415 * 2.0); theta += dtheta){
    x_array[n] = (r * cos(theta)) + x;
    y_array[n] = (r * sin(theta)) + y;
    n++;
  }
  return Gi_fill_polygon(x_array, y_array, n);
}

int G_clear(){
  return SDL_RenderClear(renderer_);
}

int GetFontPixelHeight(){
  return font_pt_;
}

int GetStringLength(const char* str){
  int w, h;
  GetStringSize(str, &w, &h);
  return w;
}

int GetStringSize(const char* str, int* w, int *h){
  if(font_ == NULL){
    font_ = TTF_OpenFont("roboto.ttf", font_pt_);
    if(font_ == NULL){
      printf("Failed to open \"roboto.ttf\"!");
      return 0;
    }
  }
  TTF_SizeUTF8(font_, str, w, h);
  return 1;
}

int DrawString(const char* str, int x, int y){
  if(font_ == NULL){
    font_ = TTF_OpenFont("roboto.ttf", font_pt_);
    if(font_ == NULL){
      printf("Failed to open \"roboto.ttf\"!\n");
      return 0;
    }
  }
  SDL_Color color;
  SDL_GetRenderDrawColor(renderer_, &color.r, &color.g, &color.b, &color.a);
  SDL_Surface* text = TTF_RenderUTF8_Solid(font_, str, color);
  SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer_, text);
  SDL_Rect dest = {x, size_[1] - y, text->w, text->h};
  SDL_RenderCopy(renderer_, text_texture, NULL, &dest);
  SDL_FreeSurface(text);
  SDL_DestroyTexture(text_texture);
  RenderPresent();
  return 1;
}

int SetFontPt(int pt){
  font_pt_ = pt;
  return 1;
}

int Text(int linec, const char* text, double startx, double starty, double height, double width, double kerning, double spacing){
  return 0;
}

int G_save_image_to_file(const char* filename){
  SDL_Texture* ren_tex;
  SDL_Surface *surf;
  int st, w, h, format;
  void *pixels;
  pixels =NULL;
  surf = NULL;
  ren_tex = NULL;
  format = SDL_PIXELFORMAT_RGBA32;

  st = SDL_QueryTexture(display_, NULL, NULL, &w, &h);
  if (st != 0){
    SDL_Log("failed querying texture: %s\n", SDL_GetError());
        goto cleanup;
  }
  ren_tex = SDL_CreateTexture(renderer_, format,SDL_TEXTUREACCESS_TARGET, w, h );
  if (!ren_tex){
    SDL_Log("Failed creating render texture: %s\n", SDL_GetError());
        goto cleanup;
  }
  st = SDL_SetRenderTarget(renderer_, ren_tex);
    if (st != 0) {
        SDL_Log("Failed setting render target: %s\n", SDL_GetError());
        goto cleanup;
    }

    SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer_);

    st = SDL_RenderCopy(renderer_,display_, NULL, NULL);
    if (st != 0) {
        SDL_Log("Failed copying texture data: %s\n", SDL_GetError());
        goto cleanup;
    }

    /* Create buffer to hold texture data and load it */
    pixels = malloc(w * h * SDL_BYTESPERPIXEL(format));
    if (!pixels) {
        SDL_Log("Failed allocating memory\n");
        goto cleanup;
    }

    st = SDL_RenderReadPixels(renderer_, NULL, format, pixels, w * SDL_BYTESPERPIXEL(format));
    if (st != 0) {
        SDL_Log("Failed reading pixel data: %s\n", SDL_GetError());
        goto cleanup;
    }

    /* Copy pixel data over to surface */
    surf = SDL_CreateRGBSurfaceWithFormatFrom(pixels, w, h, SDL_BITSPERPIXEL(format), w * SDL_BYTESPERPIXEL(format), format);
    if (!surf) {
        SDL_Log("Failed creating new surface: %s\n", SDL_GetError());
        goto cleanup;
    }

    /* Save result to an image */
    st = SDL_SaveBMP(surf, filename);
    if (st != 0) {
        SDL_Log("Failed saving image: %s\n", SDL_GetError());
        goto cleanup;
    }

cleanup:
    SDL_FreeSurface(surf);
    free(pixels);
    SDL_DestroyTexture(ren_tex);
  return 0;
}
