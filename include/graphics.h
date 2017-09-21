#ifndef FPT_GRAPHICS_H
#define FPT_GRAPHICS_H

int InitFPT(unsigned int w, unsigned int h);
int TermFPT();

int ShouldClose();
int SetShouldClose(int should_close);
int GetCurrentSize(int size[2]);
int DGetCurrentSize(double size[2]);

int RenderPresent();

int ISetColorRgb(int r, int g, int b);
int DSetColorRgb(double r, double g, double b);

int Point(int x, int y);
// Deprecated
int SafePoint(int x, int y);
int Line(int x1, int y1, int x2, int y2);
// Deprecated
int SafeLine(int x1, int y1, int x2, int y2);
int Rectangle(int x, int y, int w, int h);
int FillRectangle(int x, int y, int w, int h);
int Triangle(int x1, int y1, int x2, int y2, int x3, int y3);
int FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3);
int Polygon(int* x, int* y, int n);
int DPolygon(double* x, double* y, int n);
int FillPolygon(int* x, int* y, int n);
int DFillPolygon(double* x, double* y, int n);
int Circle(int x, int y, int r);
int FillCircle(int a, int b, int r);

int GetFontPixelHeight();
int GetStringLength(const char*);
int GetStringSize(const char* str, int* w, int* h);
int DrawString(const char* str, int x, int y);
int SetFontPt(int pt);

// Uninplemented
int SetPenSize(int w, int h);

// Uninplemented
int Text(int linec, const char* text, double startx, double starty, double height, double width, double kerning, double spacing);

// Uninplemented
int SaveImageToFile(const char* file);

#endif /* ifndef FPT_GRAPHICS_H */
