#ifndef FPT
#define FPT

#include <stdint.h>
#include <stdio.h>

#ifdef __linux__
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#endif

static int __win_width;
static int __win_height;
static int __pix_width;
static int __pix_height;
static int __tmp_width;
static int __tmp_height;
static int __depth;

#ifdef __linux__
static Display* __display;
static Window __root_window;
static int __screen_number;
static Window __window;
static Pixmap __pixmap;
static Pixmap __tmp_pixmap;
static XID __drawable;
static GC __window_context;
static GC __pixmap_context;
static GC __tmp_contexxt;
#endif

static int __red;
static int __green;
static int __blue;
static uint32_t __current_color_pixel;
static uint32_t __last_clear_buffer_pixel;

#ifdef __linux__
void x_clear_buffer() {
  XFillRectangle(__display, __drawable, __pixmap_context, 0, 0, __pix_width,
                 __pix_height);
  XFlush(__display);
  __last_clear_buffer_pixel = __current_color_pixel;
}

void x_copy_buffer() {
  XCopyArea(__display, __pixmap, __window, __window_context, 0,
            __pix_height - __win_height, __win_width, __win_height, 0, 0);
}

void x_set_color_rgb(int r, int g, int b) {
  r = (r < 0) ? 0 : (r > 255) ? 255 : r;
  g = (g < 0) ? 0 : (g > 255) ? 255 : g;
  b = (b < 0) ? 0 : (b > 255) ? 255 : b;
  uint32_t color = (r << 16) | (g << 8) | (b);
  XSetForeground(__display, __pixmap_context, color);
  __red = r;
  __green = g;
  __blue = b;
  __current_color_pixel = color;
}

void x_copy_buffer_and_flush() {
  XCopyArea(__display, __pixmap, __window, __window_context, 0,
            __pix_height - __win_height, __win_width, __win_height, 0, 0);
  XFlush(__display);
}

int x_handle_events(int* px, int* py) {
  int x, y;
  int retval;
  static int buff_size = 10;
  char buffer[buff_size];
  XEvent event;
  KeySym keysym;
  XComposeStatus compose;
  *px = 0;
  *py = 0;
X_MORE_EVENTS:
  if (!XPending(__display)) {
    return -3000;
  }
  XNextEvent(__display, &event);
  switch (event.type) {
    case Expose:
      x_copy_buffer_and_flush();
      return -1;
    case MotionNotify:
      x = event.xmotion.x;
      y = __pix_height - 1 - event.xmotion.y;
      if (event.xmotion.state & Button3Mask ||
          event.xmotion.state & Button1Mask) {
        *px = x;
        *py = y;
        return -2;
      }
      goto X_MORE_EVENTS;
    case ButtonPress:
      x = event.xbutton.x;
      y = __win_height - 1 - event.xbutton.y;
      *px = x;
      *py = y;
      return -3;
    case KeyPress:
      XLookupString(&(event.xkey), buffer, buff_size, &keysym, &compose);
      return keysym;
    default:
      return -1000;
  }
  return -2000;
}

int x_get_events(int* d) {
  int s, x, y;
  s = x_handle_events(&x, &y);
  d[0] = x;
  d[1] = y;
  return s;
}

void x_point(double dx, double dy) {
  XDrawPoint(__display, __drawable, __pixmap_context, (int)dx,
             __pix_height - 1 - (int)dy);
}
void x_safe_point(double dx, double dy) {
  int x = (int)dx;
  int y = (int)dy;
  if ((x < 0) || (y < 0) || (x >= __pix_width) || (y >= __pix_height)) return;
  XDrawPoint(__display, __drawable, __pixmap_context, x, __pix_height - 1 - y);
}

void x_line(double dxs, double dys, double dxe, double dye) {
  XDrawLine(__display, __drawable, __pixmap_context, (int)dxs,
            __pix_height - 1 - (int)dys, (int)dxe, __pix_height - 1 - (int)dye);
}

void x_safe_line(double dxs, double dys, double dxe, double dye) {
  int ixs = (int)dxs;
  int iys = (int)dys;
  int ixe = (int)dxe;
  int iye = (int)dye;

  double xs, ys, xe, ye;  // doubles for accuracy in clipping
  double t, xedge, yedge;

  if ((ixs >= 0) && (ixs < __pix_width) && (ixe >= 0) && (ixe < __pix_width) &&
      (iys >= 0) && (iys < __pix_height) && (iye >= 0) &&
      (iye < __pix_height)) {
    XDrawLine(__display, __drawable, __pixmap_context, ixs,
              __pix_height - 1 - iys, ixe, __pix_height - 1 - iye);
    return;
  }
  xs = ixs;
  ys = iys;
  xe = ixe;
  ye = iye;
  yedge = 0;
  if (ys >= yedge) {
    if (ye >= yedge) {
    } else {
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
      return;
    }
  }
  yedge = __pix_height - 1;
  if (ys <= yedge) {
    if (ye <= yedge) {
    } else {
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
      return;
    }
  }
  xedge = 0;
  if (xs >= xedge) {
    if (xe >= xedge) {
    } else {
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
      return;
    }
  }
  xedge = __pix_width - 1;
  if (xs <= xedge) {
    if (xe <= xedge) {
    } else {
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
      return;
    }
  }
  XDrawLine(__display, __drawable, __pixmap_context, (int)xs,
            (int)(__pix_height - 1 - ys), (int)xe,
            (int)(__pix_height - 1 - ye));
}

int x_init(double width, double height) {
  int screen_width = (int)width;
  int screen_height = (int)height;

  __display = XOpenDisplay("\0");
  __root_window = DefaultRootWindow(__display);
  __screen_number = DefaultScreen(__display);
  __depth = DefaultDepth(__display, __screen_number);
  if (__depth != 24) {
    printf("24 bit color not supported.\n");
    printf("Color function not likely to work.\n");
    return 0;
  }
  __pix_width = screen_width;
  __pix_height = screen_height;
  __win_width = screen_width;
  __win_height = screen_height;

  int max_width, max_height;
  max_width = XDisplayWidth(__display, __screen_number);
  max_height = XDisplayHeight(__display, __screen_number);
  if (screen_width > max_width) {
    printf("Requested width of %d exceeds max allowable of %d.\n", screen_width,
           max_width);

    printf("Xx display window width truncated\n");
    printf("BUT the back buffer IS as specified.\n");
    __win_width = max_width;
  }

  if (screen_height > max_height) {
    printf("Requested height of %d exceeds max allowable of %d.\n",
           screen_height, max_height);

    printf("Xx display window width truncated\n");
    printf("BUT the back buffer IS as specified.\n");
    __win_height = max_height;
  }

  __window = XCreateSimpleWindow(__display, __root_window, 0, 0, __win_width,
                                 __win_height, 0, 0, 0);
  if (!__window) return 0;
  __pixmap = XCreatePixmap(__display, __root_window, __pix_width, __pix_height,
                           __depth);
  if (!__pixmap) return 0;
  __drawable = __pixmap;

  XGCValues vals;
  XMapWindow(__display, __window);
  XSelectInput(__display, __window,
               ExposureMask | StructureNotifyMask | PointerMotionMask |
                   ButtonPressMask | KeyPressMask);
  vals.graphics_exposures = 0;
  __window_context = XCreateGC(__display, __window, GCGraphicsExposures, &vals);
  if (!__window_context) return 0;
  __pixmap_context = XCreateGC(__display, __pixmap, 0, 0);
  if (!__pixmap_context) return 0;

  x_set_color_rgb(255, 255, 255);
  x_clear_buffer();
  x_copy_buffer();
  XFlush(__display);
  x_set_color_rgb(0, 0, 0);
  return 1;
}

void x_term() {
  XDestroyWindow(__display, __window);
  XFreeGC(__display, __window_context);
  XFreeGC(__display, __pixmap_context);
  XFreePixmap(__display, __pixmap);
  XCloseDisplay(__display);
}
#endif

void (*G_close)();
void (*G_display_image)();
int (*Gi_events)(int*);
void (*Gi_rgb)(int, int, int);

void(*G_pixel)(double, double);
void(*G_point)(double, double);

int G_init_graphics(double w, double h) {
#ifdef __linux__
  G_close = x_term;
  G_display_image = x_copy_buffer_and_flush;
  Gi_events = x_get_events;
  Gi_rgb = x_set_color_rgb;
  G_pixel = x_point;
  G_point = x_safe_point;
  return x_init(w, h);
#endif
}

int G_wait_key() {
  int p[2];
  int sig;
  G_display_image();
  do {
    sig = Gi_events(p);
  } while (sig < 0);
  return sig;
}

#endif  // FPT
