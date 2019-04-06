#include "fpt.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef FPT_X11
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

static unsigned int width_, height_;

static Display* display_;
static Window window_;
static Pixmap pixmap_;
static GC window_context_;
static GC pixmap_context_;

void G_version(unsigned* major, unsigned* minor) {
  *major = FPT_VERSION_MAJOR;
  *minor = FPT_VERSION_MINOR;
}

void G_sleep(double sec) {
  nanosleep(&(struct timespec){(int)sec, (long)(1e9 * (sec - (int)sec))}, NULL);
}

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
  /* XClearArea(display_, window_, 0, 0, 0, 0, True); */
  XSetForeground(display_, pixmap_context_, 0x000000);
  XFillRectangle(display_, pixmap_, pixmap_context_, 0, 0, width, height);
  XCopyArea(display_, pixmap_, window_, window_context_, 0, 0, width, height, 0,
            0);
  XFlush(display_);
  /* XSync(display_, False); */
  /* XFillRectangle(display_, pixmap_, pixmap_context_, 0, 0, width, height); */
  /* nanosleep(&(struct timespec){(int)1, (long)50000000}, NULL); */
  /* int sig = 0; */
  /* do { */
  /*   sig = G_handle_events(NULL); */
  /*   printf(">>%d\n", sig); */
  /* } while (sig != 0); */
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

  return 0;
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
    if (fabs(e2) < fabs(e1)) {
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
    if (fabs(e2) < fabs(e1)) {
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
    /* printf(">>%d\n", sig); */
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
