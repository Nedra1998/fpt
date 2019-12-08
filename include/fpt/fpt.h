#ifndef FPT_H_
#define FPT_H_

#include <stdbool.h>
#include <stdlib.h>

bool G_init_graphics(size_t w, size_t h);
bool G_close();
void G_display_image();
void G_clear();
void G_rgb(double red, double green, double blue);
void G_set_pen_size(double size);
void G_point(double x, double y);
void G_pixel(double x, double y);
void G_line(double sx, double sy, double ex, double ey);
void G_rectangle(double x, double y, double w, double h);
void G_fill_rectangle(double x, double y, double w, double h);
void G_triangle(double x1, double y1, double x2, double y2, double x3, double y3);
void G_fill_triangle(double x1, double y1, double x2, double y2, double x3,
                     double y3);
void G_polygon(double* x, double* y, size_t n);
void G_fill_polygon(double* x, double* y, size_t n);
void G_circle(double cx, double cy, double r);
void G_fill_circle(double cx, double cy, double r);
int G_handle_events(double* px, double* py);
int G_events(double* pos);
int G_wait_event(double pos[2]);
int G_wait_key();
int G_wait_click(double pos[2]);
void Gi_get_current_window_dimensions(size_t* dim);
bool G_save_image_to_file(const char* path);

#endif  // FPT_H_
