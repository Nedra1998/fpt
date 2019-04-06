#ifndef FPT_H_
#define FPT_H_

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define FPT_VERSION_MAJOR 2
#define FPT_VERSION_MINOR 0

void G_version(unsigned* major, unsigned* minor);
void G_sleep(double s);

int G_init_graphics(unsigned int width, unsigned int height);
int G_term_graphics();

void G_display_image();

void G_hex(unsigned long c);
void G_rgb(double r, double g, double b);
void Gi_rgb(int r, int g, int b);

int G_pixel(int x, int y);
int G_point(int x, int y);

int G_line(int x0, int y0, int x1, int y1);
int G_triangle(int x0, int y0, int x1, int y1, int x2, int y2);
int G_rectangle(int xleft, int yleft, int width, int height);
int G_circle(int a, int b, int r);
int G_polygon(double* x, double* y, int numpts);
int Gi_polygon(int * x, int * y, int numpts);

int G_fill_triangle(int x0, int y0, int x1, int y1, int x2, int y2);
int G_fill_rectangle(int xleft, int yleft, int width, int height);
int G_fill_circle(int a, int b, int r);
int G_fill_polygon(double* x, double* y, int numpts);
int Gi_fill_polygon(int * x, int * y, int numpts);

int G_single_pixel_horizontal_line(int x0, int x1, int y);

int G_clear();

int G_save_image_to_file(void* filename);

int G_handle_events(int* p);
void G_clear_events();

int Gi_wait_click(int p[2]);
int G_wait_click(double p[2]);
int G_wait_key();

int Gi_poll_click(int p[2]);
int G_poll_click(double p[2]);
int G_poll_key();


#endif  // FPT_H_
