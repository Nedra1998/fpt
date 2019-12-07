#ifndef FPT_H_
#define FPT_H_

#include <stdlib.h>
#include <stdbool.h>

bool G_init_graphics(size_t w, size_t h);
bool G_close();
void G_display_image();
void G_clear();
void G_rgb(float red, float green, float blue);
void G_set_pen_size(float size);
void G_point(float x, float y);
void G_line(float sx, float sy, float ex, float ey);
void G_rectangle(float x, float y, float w, float h);
void G_fill_rectangle(float x, float y, float w, float h);
void G_triangle(float x1, float y1, float x2, float y2, float x3, float y3);
void G_fill_triangle(float x1, float y1, float x2, float y2, float x3,
                     float y3);
int G_handle_events(float* px, float* py);
int G_events(float* pos);
int G_wait_key();
void Gi_get_current_window_dimensions(size_t* dim);

#endif  // FPT_H_
