#include "fpt.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static unsigned int width_, height_;

void G_version(unsigned* major, unsigned* minor) {
  *major = FPT_VERSION_MAJOR;
  *minor = FPT_VERSION_MINOR;
}

void G_sleep(double sec) {
  nanosleep(&(struct timespec){(int)sec, (long)(1e9 * (sec - (int)sec))}, NULL);
}

#ifdef FPT_X11
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

static Display* display_;
static Window window_;
static Pixmap pixmap_;
static GC window_context_;
static GC pixmap_context_;

int G_init_graphics(unsigned int width, unsigned int height) {
  width_ = width;
  height_ = height;
  if (display_ == NULL) {
    display_ = XOpenDisplay(NULL);
  }
  if (display_ == NULL) return 0;
  window_ = XCreateSimpleWindow(display_, DefaultRootWindow(display_), 0, 0,
                                width, height, 0, 0x000000, 0x000000);
  if (window_ == 0) return 0;
  pixmap_ = XCreatePixmap(display_, DefaultRootWindow(display_), width, height,
                          DefaultDepth(display_, DefaultScreen(display_)));
  if (pixmap_ == 0) return 0;
  XMapWindow(display_, window_);
  XSelectInput(display_, window_,
               ExposureMask | StructureNotifyMask | PointerMotionMask |
                   ButtonPressMask | KeyPressMask);
  XGCValues vals;
  vals.graphics_exposures = 0;
  window_context_ = XCreateGC(display_, window_, GCGraphicsExposures, &vals);
  if (window_context_ == NULL) return 0;
  pixmap_context_ = XCreateGC(display_, pixmap_, 0, NULL);
  if (pixmap_context_ == NULL) return 0;
  XClearWindow(display_, window_);
  XSetForeground(display_, pixmap_context_, 0x000000);
  XFillRectangle(display_, pixmap_, pixmap_context_, 0, 0, width, height);
  XCopyArea(display_, pixmap_, window_, window_context_, 0, 0, width, height, 0,
            0);
  XFlush(display_);
  XSetForeground(display_, pixmap_context_, 0xffffff);
  return 1;
}

int G_term_graphics() {
  XDestroyWindow(display_, window_);
  XFreeGC(display_, window_context_);
  XFreeGC(display_, pixmap_context_);
  XFreePixmap(display_, pixmap_);
  XCloseDisplay(display_);
  return 1;
}

void G_display_image() {
  XCopyArea(display_, pixmap_, window_, window_context_, 0, 0, width_, height_,
            0, 0);
  XFlush(display_);
}

void G_hex(unsigned long c) {
  XSetForeground(display_, pixmap_context_, (c > 0xffffff) ? 0xffffff : c);
}
void G_rgb(double r, double g, double b) {
  int ir = (int)(0xff * r);
  int ig = (int)(0xff * g);
  int ib = (int)(0xff * b);
  XSetForeground(display_, pixmap_context_,
                 (((ir < 0) ? 0 : (ir > 0xff) ? 0xff : ir) << 16) |
                     (((ig < 0) ? 0 : (ig > 0xff) ? 0xff : ig) << 8) |
                     (((ib < 0) ? 0 : (ib > 0xff) ? 0xff : ib)));
}
void Gi_rgb(int r, int g, int b) {
  XSetForeground(display_, pixmap_context_,
                 (((r < 0) ? 0 : (r > 0xff) ? 0xff : r) << 16) |
                     (((g < 0) ? 0 : (g > 0xff) ? 0xff : g) << 8) |
                     (((b < 0) ? 0 : (b > 0xff) ? 0xff : b)));
}

int G_pixel(int x, int y) {
  XDrawPoint(display_, pixmap_, pixmap_context_, x, height_ - 1 - y);
  return 1;
}
int G_point(int x, int y) {
  if (x < 0 || x >= width_ || y < 0 || y >= height_) return 0;
  XDrawPoint(display_, pixmap_, pixmap_context_, x, height_ - 1 - y);
  return 1;
}

int G_line(int x0, int y0, int x1, int y1) {
  double xs, ys, xe, ye;
  double t, xedge, yedge;
  if (x0 >= 0 && x0 < width_ && y0 >= 0 && y0 < height_ && x1 >= 0 &&
      x1 < width_ && y1 >= 0 && y1 < height_) {
    XDrawLine(display_, pixmap_, pixmap_context_, x0, height_ - 1 - y0, x1,
              height_ - 1 - y1);
    return 1;
  }

  xs = (double)x0;
  ys = (double)y0;
  xe = (double)x1;
  ye = (double)y1;

  yedge = 0;
  if (ys >= yedge) {
    if (ye < yedge) {
      t = (yedge - ys) / (ye - ys);
      xe = xs + t * (xe - xs);
      ye = yedge;
    }
  } else {
    if (ye >= yedge) {
      t = (yedge - ys) / (ye - ys);
      xs = xs + t * (xe - xs);
      ys = yedge;
    } else {
      return 0;
    }
  }
  yedge = height_ - 1;
  if (ys <= yedge) {
    if (ye > yedge) {
      t = (yedge - ys) / (ye - ys);
      xe = xs + t * (xe - xs);
      ye = yedge;
    }
  } else {
    if (ye <= yedge) {
      t = (yedge - ys) / (ye - ys);
      xs = xs + t * (xe - xs);
      ys = yedge;
    } else {
      return 0;
    }
  }
  xedge = 0;
  if (xs >= xedge) {
    if (xe < xedge) {
      t = (xedge - xs) / (xe - xs);
      ye = ys + t * (ye - ys);
      xe = xedge;
    }
  } else {
    if (xe >= xedge) {
      t = (xedge - xs) / (xe - xs);
      ys = ys + t * (ye - ys);
      xs = xedge;
    } else {
      return 0;
    }
  }
  xedge = width_ - 1;
  if (xs <= xedge) {
    if (xe > xedge) {
      t = (xedge - xs) / (xe - xs);
      ye = ys + t * (ye - ys);
      xe = xedge;
    }
  } else {
    if (xe <= xedge) {
      t = (xedge - xs) / (xe - xs);
      ys = ys + t * (ye - ys);
      xs = xedge;
    } else {
      return 0;
    }
  }

  XDrawLine(display_, pixmap_, pixmap_context_, (int)xs, height_ - 1 - (int)ys,
            (int)xe, height_ - 1 - (int)ye);

  return 1;
}

int G_triangle(int x0, int y0, int x1, int y1, int x2, int y2) {
  XDrawLines(display_, pixmap_, pixmap_context_,
             (XPoint[4]){{x0, height_ - 1 - y0},
                         {x1, height_ - 1 - y1},
                         {x2, height_ - 1 - y2},
                         {x0, height_ - 1 - y0}},
             4, CoordModeOrigin);
  return 1;
}
int G_rectangle(int xlow, int ylow, int width, int height) {
  XDrawRectangle(display_, pixmap_, pixmap_context_, xlow,
                 height_ - 1 - ylow - height, width, height);
  return 1;
}
int G_circle(int a, int b, int r) {
  int x, y, e;
  x = r;
  y = 0;
  e = 0;
  while (x >= y) {
    int e1, e2;
    G_point(a + x, b + y);
    G_point(a - x, b + y);
    G_point(a + x, b - y);
    G_point(a - x, b - y);

    G_point(a + y, b + x);
    G_point(a - y, b + x);
    G_point(a + y, b - x);
    G_point(a - y, b - x);
    e1 = e + y + y + 1;
    e2 = e1 - x - x + 1;
    y = y + 1;
    if (abs(e2) < abs(e1)) {
      x = x - 1;
      e = e2;
    } else {
      e = e1;
    }
  }
  return 1;
}
int G_polygon(double* x, double* y, int numpts) {
  if (numpts <= 0) return 0;
  XPoint* points = (XPoint*)malloc(numpts * sizeof(XPoint));
  for (int i = 0; i < numpts; ++i) {
    points[i] = (XPoint){(int)x[i], height_ - 1 - (int)y[i]};
  }
  XDrawLines(display_, pixmap_, pixmap_context_, points, numpts,
             CoordModeOrigin);
  XDrawLine(display_, pixmap_, pixmap_context_, points[0].x, points[0].y,
            points[numpts - 1].x, points[numpts - 1].y);
  free(points);
  return 1;
}
int Gi_polygon(int* x, int* y, int numpts) {
  if (numpts <= 0) return 0;
  XPoint* points = (XPoint*)malloc(numpts * sizeof(XPoint));
  for (int i = 0; i < numpts; ++i) {
    points[i] = (XPoint){x[i], height_ - 1 - y[i]};
  }
  XDrawLines(display_, pixmap_, pixmap_context_, points, numpts,
             CoordModeOrigin);
  XDrawLine(display_, pixmap_, pixmap_context_, points[0].x, points[0].y,
            points[numpts - 1].x, points[numpts - 1].y);
  free(points);
  return 1;
}

int G_fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2) {
  XFillPolygon(display_, pixmap_, pixmap_context_,
               (XPoint[3]){{x0, height_ - 1 - y0},
                           {x1, height_ - 1 - y1},
                           {x2, height_ - 1 - y2}},
               3, Convex, CoordModeOrigin);
  return 1;
}
int G_fill_rectangle(int xlow, int ylow, int width, int height) {
  XFillRectangle(display_, pixmap_, pixmap_context_, xlow,
                 height_ - 1 - ylow - height, width, height);
  return 1;
}
int G_fill_circle(int a, int b, int r) {
  int x, y, e;
  x = r;
  y = 0;
  e = 0;
  while (x >= y) {
    int e1, e2;
    G_single_pixel_horizontal_line(a - x, a + x, b + y);
    G_single_pixel_horizontal_line(a - x, a + x, b - y);

    G_single_pixel_horizontal_line(a - y, a + y, b + x);
    G_single_pixel_horizontal_line(a - y, a + y, b - x);
    e1 = e + y + y + 1;
    e2 = e1 - x - x + 1;
    y = y + 1;
    if (abs(e2) < abs(e1)) {
      x = x - 1;
      e = e2;
    } else {
      e = e1;
    }
  }
  return 1;
}
int G_fill_polygon(double* x, double* y, int numpts) {
  if (numpts <= 0) return 0;
  XPoint* points = (XPoint*)malloc(numpts * sizeof(XPoint));
  for (int i = 0; i < numpts; ++i) {
    points[i] = (XPoint){(int)x[i], height_ - 1 - (int)y[i]};
  }
  XFillPolygon(display_, pixmap_, pixmap_context_, points, numpts, Nonconvex,
               CoordModeOrigin);
  free(points);
  return 1;
}
int Gi_fill_polygon(int* x, int* y, int numpts) {
  if (numpts <= 0) return 0;
  XPoint* points = (XPoint*)malloc(numpts * sizeof(XPoint));
  for (int i = 0; i < numpts; ++i) {
    points[i] = (XPoint){x[i], height_ - 1 - y[i]};
  }
  XFillPolygon(display_, pixmap_, pixmap_context_, points, numpts, Nonconvex,
               CoordModeOrigin);
  free(points);
  return 1;
}

int G_single_pixel_horizontal_line(int x0, int x1, int y) {
  if (y < 0 || y >= height_) return 0;
  if (x0 < 0)
    x0 = 0;
  else if (x0 >= width_)
    x0 = width_ - 1;
  if (x1 < 0)
    x1 = 0;
  else if (x1 >= width_)
    x1 = width_ - 1;
  if (x1 < x0) {
    int t;
    t = x1;
    x1 = x0;
    x0 = t;
  }
  XDrawLine(display_, pixmap_, pixmap_context_, (int)x0, height_ - 1 - (int)y,
            (int)x1, height_ - 1 - (int)y);
  return 1;
}

int G_clear() {
  XFillRectangle(display_, pixmap_, pixmap_context_, 0, 0, width_, height_);
  XFlush(display_);
  return 1;
}

void fputintB(int x, FILE* fp) {
  char* p;
  int c[4], i;
  p = (char*)&x;
  for (i = 0; i < 4; i++) {
    c[i] = *p;
    p++;
  }
  for (i = 3; i >= 0; i--) {
    fputc(c[i], fp);
  }
}

void write_xwd(XImage* pxim, FILE* fp) {
  int header_size = 104, file_version = 7, format = 2, depth = 24, xoffset = 0,
      byte_order = 0, bitmap_unit = 32, bitmap_order = 0, bitmap_pad = 32,
      bits_per_pixel = 32, bytes_per_line = pxim->width * 4, visual_class = 5,
      redmask = 0x00ff0000, greenmask = 0x0000ff00, bluemask = 0x000000ff,
      bitsperrgb = 24, numentriesincolormap = 0, numcolorstructures = 0,
      windowwidth = pxim->width, windowheight = pxim->height, windowx = 0,
      windowy = 0, windowborderwidth = 0, width = pxim->width,
      height = pxim->height;
  int numbytestowrite = bytes_per_line * height;
  fputintB(header_size, fp);
  fputintB(file_version, fp);
  fputintB(format, fp);
  fputintB(depth, fp);
  fputintB(width, fp);
  fputintB(height, fp);
  fputintB(xoffset, fp);
  fputintB(byte_order, fp);
  fputintB(bitmap_unit, fp);
  fputintB(bitmap_order, fp);
  fputintB(bitmap_pad, fp);
  fputintB(bits_per_pixel, fp);
  fputintB(bytes_per_line, fp);
  fputintB(visual_class, fp);
  fputintB(redmask, fp);
  fputintB(greenmask, fp);
  fputintB(bluemask, fp);
  fputintB(bitsperrgb, fp);
  fputintB(numentriesincolormap, fp);
  fputintB(numcolorstructures, fp);
  fputintB(windowwidth, fp);
  fputintB(windowheight, fp);
  fputintB(windowx, fp);
  fputintB(windowy, fp);
  fputintB(windowborderwidth, fp);
  fputc('\0', fp);
  fputc('\0', fp);
  fputc('\0', fp);
  fputc('\0', fp);
  fwrite(pxim->data, numbytestowrite, 1, fp);
}

int G_save_image_to_file(void* filename) {
  // TODO add BMP writing
  FILE* fp;
  XImage* pxim;
  fp = fopen((char*)filename, "w");
  if (fp == NULL) {
    fprintf(stderr, "G_save_image_to_file cannor open file %s\n",
            (char*)filename);
    return 0;
  }
  pxim =
      XGetImage(display_, pixmap_, 0, 0, width_, height_, AllPlanes, ZPixmap);

  write_xwd(pxim, fp);

  fclose(fp);
  return 1;
}

int G_handle_events(int* p) {
  KeySym keysym;
  char buffer[10];
  int bufsize = 10;
  /* XComposeStatus compose; */
  XComposeStatus compose;
  do {
    XEvent event;
    XNextEvent(display_, &event);
    /* printf(">>>%d/%d<<<<\n", event.type, KeyPress); */
    switch (event.type) {
      case Expose:
        G_display_image();
        break;
      case MotionNotify:
        if (event.xmotion.state & Button3Mask && p != NULL) {
          p[0] = event.xmotion.x;
          p[1] = height_ - 1 - event.xmotion.y;
        } else {
          if (event.xmotion.state & Button1Mask && p != NULL) {
            p[0] = event.xmotion.x;
            p[1] = height_ - 1 - event.xmotion.y;
          } else {
            break;
          }
          return -1;
        }
      case ButtonPress:
        if (p != NULL) {
          p[0] = event.xbutton.x;
          p[1] = height_ - 1 - event.xbutton.y;
          return -2;
        } else {
          return -2;
        }
      case KeyPress:
        XLookupString(&(event.xkey), buffer, bufsize, &keysym, &compose);
        return keysym;
      default:
        return 0;
    }
  } while (XPending(display_));
  return 0;
}

void G_clear_events() {
  G_display_image();
  while (G_handle_events(NULL) != 0) {
  }
}

int Gi_wait_click(int p[2]) {
  G_display_image();
  int sig;
  do {
    sig = G_handle_events(p);
  } while (sig != -2);
  return 1;
}
int G_wait_click(double p[2]) {
  G_display_image();
  int sig;
  int ip[2];
  do {
    sig = G_handle_events(ip);
  } while (sig != -2);
  p[0] = (double)ip[0];
  p[1] = (double)ip[1];
  return 1;
}
int G_wait_key() {
  G_display_image();
  int sig;
  while ((sig = G_handle_events(NULL)) <= 0) {
  }
  return sig;
}

int Gi_poll_click(int p[2]) {
  G_display_image();
  p[0] = 0;
  p[1] = 0;
  return (G_handle_events(p) == -2);
}
int G_poll_click(double p[2]) {
  G_display_image();
  int sig;
  int ip[2] = {0, 0};
  sig = G_handle_events(ip);
  p[0] = (double)ip[0];
  p[1] = (double)ip[1];
  return sig == -2;
}
int G_poll_key() {
  G_display_image();
  return G_handle_events(NULL);
}
#endif
#ifdef FPT_SDL

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>

static SDL_Window* window_ = NULL;
static SDL_Renderer* renderer_ = NULL;
static SDL_Texture* display_ = NULL;

int G_init_graphics(unsigned int width, unsigned int height) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    return 0;
  }
  if (TTF_Init() == -1) {
    return 0;
  }
  int flags = IMG_INIT_PNG | IMG_INIT_JPG;
  int initted = IMG_Init(flags);
  if (initted & flags != flags) {
    return 0;
  }
  window_ = SDL_CreateWindow("FPT", 0, 0, width, height, 0);
  if (window_ == NULL) {
    return 0;
  }
  renderer_ = SDL_CreateRenderer(
      window_, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
  if (renderer_ == NULL) {
    return 0;
  }
  SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
  display_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGBA8888,
                               SDL_TEXTUREACCESS_TARGET, width, height);
  SDL_SetRenderTarget(renderer_, display_);
  SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
  SDL_RenderClear(renderer_);
  G_display_image();
  SDL_GetWindowSize(window_, &width_, &height_);
  return 1;
}

int G_term_graphics() {
  if (renderer_ != NULL) {
    SDL_DestroyRenderer(renderer_);
  }
  if (window_ != NULL) {
    SDL_DestroyWindow(window_);
  }
  IMG_Quit();
  TTF_Quit();
  SDL_Quit();
  return 1;
}

void G_display_image() {
  SDL_SetRenderTarget(renderer_, NULL);
  SDL_RenderCopy(renderer_, display_, NULL, NULL);
  SDL_RenderPresent(renderer_);
  SDL_SetRenderTarget(renderer_, display_);
}

void G_hex(unsigned long c) {
  SDL_SetRenderDrawColor(renderer_, (int)((c >> 16) & 0xff),
                         (int)((c >> 8) & 0xff), (int)((c)&0xff), 255);
}
void G_rgb(double r, double g, double b) {
  SDL_SetRenderDrawColor(renderer_, (int)(255 * r), (int)(255 * g),
                         (int)(255 * b), 255);
}
void Gi_rgb(int r, int g, int b) {
  SDL_SetRenderDrawColor(renderer_, r, g, b, 255);
}

int G_pixel(int x, int y) {
  return SDL_RenderDrawPoint(renderer_, x, height_ - 1 - y);
}
int G_point(int x, int y) {
  if (x < 0 || x >= width_ || y < 0 || y >= height_) return 0;
  return SDL_RenderDrawPoint(renderer_, x, height_ - 1 - y);
}

int G_line(int x0, int y0, int x1, int y1) {
  double xs, ys, xe, ye;
  double t, xedge, yedge;
  if (x0 >= 0 && x0 < width_ && y0 >= 0 && y0 < height_ && x1 >= 0 &&
      x1 < width_ && y1 >= 0 && y1 < height_) {
    return SDL_RenderDrawLine(renderer_, x0, height_ - 1 - y0, x1,
                              height_ - 1 - y1);
  }

  xs = (double)x0;
  ys = (double)y0;
  xe = (double)x1;
  ye = (double)y1;

  yedge = 0;
  if (ys >= yedge) {
    if (ye < yedge) {
      t = (yedge - ys) / (ye - ys);
      xe = xs + t * (xe - xs);
      ye = yedge;
    }
  } else {
    if (ye >= yedge) {
      t = (yedge - ys) / (ye - ys);
      xs = xs + t * (xe - xs);
      ys = yedge;
    } else {
      return 0;
    }
  }
  yedge = height_ - 1;
  if (ys <= yedge) {
    if (ye > yedge) {
      t = (yedge - ys) / (ye - ys);
      xe = xs + t * (xe - xs);
      ye = yedge;
    }
  } else {
    if (ye <= yedge) {
      t = (yedge - ys) / (ye - ys);
      xs = xs + t * (xe - xs);
      ys = yedge;
    } else {
      return 0;
    }
  }
  xedge = 0;
  if (xs >= xedge) {
    if (xe < xedge) {
      t = (xedge - xs) / (xe - xs);
      ye = ys + t * (ye - ys);
      xe = xedge;
    }
  } else {
    if (xe >= xedge) {
      t = (xedge - xs) / (xe - xs);
      ys = ys + t * (ye - ys);
      xs = xedge;
    } else {
      return 0;
    }
  }
  xedge = width_ - 1;
  if (xs <= xedge) {
    if (xe > xedge) {
      t = (xedge - xs) / (xe - xs);
      ye = ys + t * (ye - ys);
      xe = xedge;
    }
  } else {
    if (xe <= xedge) {
      t = (xedge - xs) / (xe - xs);
      ys = ys + t * (ye - ys);
      xs = xedge;
    } else {
      return 0;
    }
  }

  return SDL_RenderDrawLine(renderer_, (int)xs, height_ - 1 - (int)ys, (int)xe,
                            height_ - 1 - (int)ye);
}
int G_triangle(int x0, int y0, int x1, int y1, int x2, int y2) {
  return SDL_RenderDrawLines(renderer_,
                             (SDL_Point[4]){{x0, height_ - 1 - y0},
                                            {x1, height_ - 1 - y1},
                                            {x2, height_ - 1 - y2},
                                            {x0, height_ - 1 - y0}},
                             4);
}
int G_rectangle(int xlow, int ylow, int width, int height) {
  SDL_Rect rect = {xlow, height_ - 1 - ylow - height, width, height};
  return SDL_RenderDrawRect(renderer_, &rect);
}
int G_circle(int a, int b, int r) {
  double theta = 0, dtheta = (3.1415 / 50.0);
  SDL_Point points[100];
  int n = 0;
  for (theta = 0.0; theta < (3.1415 * 2.0); theta += dtheta) {
    points[n] = (SDL_Point){(int)(r * cos(theta)) + a,
                            height_ - 1 - ((int)(r * sin(theta)) + b)};
    n++;
  }
  int ret = SDL_RenderDrawLines(renderer_, points, 100);
  ret &= SDL_RenderDrawLine(renderer_, points[0].x, points[0].y, points[99].x,
                            points[99].y);
  return ret;
}
int G_polygon(double* x, double* y, int numpts) {
  if (numpts <= 0) return 0;
  SDL_Point* points = (SDL_Point*)malloc(numpts * sizeof(SDL_Point));
  for (int i = 0; i < numpts; ++i) {
    points[i] = (SDL_Point){(int)x[i], height_ - 1 - (int)y[i]};
  }
  int ret = SDL_RenderDrawLines(renderer_, points, numpts);
  ret &= SDL_RenderDrawLine(renderer_, points[0].x, points[0].y,
                            points[numpts - 1].x, points[numpts - 1].y);
  free(points);
  return ret;
}
int Gi_polygon(int* x, int* y, int numpts) {
  if (numpts <= 0) return 0;
  SDL_Point* points = (SDL_Point*)malloc(numpts * sizeof(SDL_Point));
  for (int i = 0; i < numpts; ++i) {
    points[i] = (SDL_Point){x[i], height_ - 1 - y[i]};
  }
  int ret = SDL_RenderDrawLines(renderer_, points, numpts);
  ret &= SDL_RenderDrawLine(renderer_, points[0].x, points[0].y,
                            points[numpts - 1].x, points[numpts - 1].y);
  free(points);
  return ret;
}

int G_fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2) {
  return Gi_fill_polygon((int[3]){x0, x1, x2}, (int[3]){y0, y1, y2}, 3);
}
int G_fill_rectangle(int xlow, int ylow, int width, int height) {
  SDL_Rect rect = {xlow, height_ - 1 - ylow - height, width, height};
  return SDL_RenderFillRect(renderer_, &rect);
}
int G_fill_circle(int a, int b, int r) {
  double theta = 0, dtheta = (3.1415 / 50.0);
  int x_array[100];
  int y_array[100];
  int n = 0;
  for (theta = 0.0; theta < (3.1415 * 2.0); theta += dtheta) {
    x_array[n] = (r * cos(theta)) + a;
    y_array[n] = (r * sin(theta)) + b;
    n++;
  }
  return Gi_fill_polygon(x_array, y_array, 100);
}
int G_fill_polygon(double* x, double* y, int n) {
  int i;
  double min = height_, max = 0;
  for (i = 0; i < n; i++) {
    if (y[i] < min) {
      min = y[i];
    }
    if (y[i] > max) {
      max = y[i];
    }
  }
  double row;
  for (row = min + 0.01; row < max; row += 1) {
    double inter[n];
    int index = 0;
    for (i = 0; i < n; i++) {
      int j = (i + 1) % n;
      if ((y[i] <= row && y[j] >= row) || (y[i] >= row && y[j] <= row)) {
        inter[index] = (row - (double)y[i]) * (((double)x[i] - (double)x[j]) /
                                               ((double)y[i] - (double)y[j])) +
                       (double)x[i];
        index++;
      }
    }
    for (i = 0; i < index; i++) {
      int j;
      for (j = 0; j < index; j++) {
        if (inter[j] > inter[i]) {
          double tmp = inter[i];
          inter[i] = inter[j];
          inter[j] = tmp;
        }
      }
    }
    for (i = 0; i < index - 1; i += 2) {
      SDL_RenderDrawLine(renderer_, (int)inter[i], height_ - 1 - (int)row,
                         (int)inter[i + 1], height_ - 1 - (int)row);
    }
  }
  return 1;
}

int Gi_fill_polygon(int* x, int* y, int n) {
  int i;
  double min = height_, max = 0;
  for (i = 0; i < n; i++) {
    if (y[i] < min) {
      min = y[i];
    }
    if (y[i] > max) {
      max = y[i];
    }
  }
  double row;
  for (row = min + 0.01; row < max; row += 1) {
    double inter[n];
    int index = 0;
    for (i = 0; i < n; i++) {
      int j = (i + 1) % n;
      if ((y[i] <= row && y[j] >= row) || (y[i] >= row && y[j] <= row)) {
        inter[index] = (row - (double)y[i]) * (((double)x[i] - (double)x[j]) /
                                               ((double)y[i] - (double)y[j])) +
                       (double)x[i];
        index++;
      }
    }
    for (i = 0; i < index; i++) {
      int j;
      for (j = 0; j < index; j++) {
        if (inter[j] > inter[i]) {
          double tmp = inter[i];
          inter[i] = inter[j];
          inter[j] = tmp;
        }
      }
    }
    for (i = 0; i < index - 1; i += 2) {
      SDL_RenderDrawLine(renderer_, (int)inter[i], height_ - 1 - (int)row,
                         (int)inter[i + 1], height_ - 1 - (int)row);
    }
  }
  return 1;
}

int Gi_wait_click(int p[2]) {
  G_display_image();
  SDL_Event event;
  while (event.type != SDL_MOUSEBUTTONDOWN) {
    SDL_WaitEvent(&event);
  }
  p[0] = event.button.x;
  p[1] = height_ - event.button.y;
  return 1;
}
int G_wait_click(double p[2]) {
  G_display_image();
  SDL_Event event;
  while (event.type != SDL_MOUSEBUTTONDOWN) {
    SDL_WaitEvent(&event);
  }
  p[0] = (double)event.button.x;
  p[1] = (double)(height_ - event.button.y);
  return 1;
}
int G_wait_key() {
  G_display_image();
  SDL_Event event;
  while (event.type != SDL_KEYDOWN) {
    SDL_WaitEvent(&event);
  }
  return event.key.keysym.sym;
}

#endif
