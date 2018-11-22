#ifndef FPT_GRAPHICS_H
#define FPT_GRAPHICS_H

void SetAutoDisplay(int val);

// G_init_graphcs
int G_init_graphics(unsigned int w, unsigned int h);
// G_term_graphics
int G_term_graphics();

int ShouldClose();
int SetShouldClose(int should_close);
int GetCurrentSize(int size[2]);
int DGetCurrentSize(double size[2]);

int RenderPresent();

// Gi_rgb
int Gi_rgb(int r, int g, int b);
// G_rgb
int G_rgb(double r, double g, double b);

// G_point
int G_point(int x, int y);
// Deprecated
int SafePoint(int x, int y);
// G_line
int G_line(int x1, int y1, int x2, int y2);
// Deprecated
int SafeLine(int x1, int y1, int x2, int y2);
// G_rectangle
int G_rectangle(int x, int y, int w, int h);
// G_fill_rectangle
int G_fill_rectangle(int x, int y, int w, int h);
// G_triangle
int G_triangle(int x1, int y1, int x2, int y2, int x3, int y3);
// G_fill_triangle
int G_fill_triangle(int x1, int y1, int x2, int y2, int x3, int y3);
// Gi_polygon
int Gi_polygon(int* x, int* y, int n);
// G_polygon
int G_polygon(double* x, double* y, int n);
// Gi_fill_polygon
int Gi_fill_polygon(int* x, int* y, int n);
// G_fill_polygon
int G_fill_polygon(double* x, double* y, int n);
// G_circle
int G_ircle(int x, int y, int r);
// G_fill_circle
int G_fill_circle(int a, int b, int r);

int G_clear();

int GetFontPixelHeight();
int GetStringLength(const char*);
int GetStringSize(const char* str, int* w, int* h);
int DrawString(const char* str, int x, int y);
int SetFontPt(int pt);

// Uninplemented
int SetPenSize(int w, int h);

// Uninplemented
int Text(int linec, const char* text, double startx, double starty, double height, double width, double kerning, double spacing);

// G_save_image_to_file
int G_save_image_to_file(const char* file);

#endif /* ifndef FPT_GRAPHICS_H */
