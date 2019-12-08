
int Font_Pixel_Height_X()
// Returns the height of the font in pixels.
{
  return XxFontInfo->max_bounds.ascent + XxFontInfo->max_bounds.descent;
}

// the void * delcarations below allow for the passing of
// arrays of characters (usual usage) but also, for instance,
// an array of doubles that is composed of packed characters

int String_Pixel_Width_X(const void* s)
// Returns the length, in pixels, of the string s
{
  int len;

  len = strlen((char*)s);

  return XTextWidth(XxFontInfo, (char*)s, len);
}

void Draw_String_X(const void* s, double Dx, double Dy)
// Draw the string s, with the lower left hand corner at (x,y)
{
  int x = (int)Dx;
  int y = (int)Dy;

  int len;

  len = strlen((const char*)s);

  XDrawString(XxDisplay, XxDrawable, XxPixmapContext, x, Xx_Pix_height - 1 - y,
              (char*)s, len);
}

void Get_Current_Dimensions_X(int dimensions[2]) {
  dimensions[0] = Xx_Pix_width;
  dimensions[1] = Xx_Pix_height;
}

void Get_Current_Dimensions_DX(double dimensions[2]) {
  dimensions[0] = Xx_Pix_width;
  dimensions[1] = Xx_Pix_height;
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void XImage_To_XWD_File(XImage* pxim, FILE* fp) {
  // assumes full color but uses full 32 bits per pixel
  int header_size, file_version, format, depth;
  int xoffset, byte_order, bitmap_unit, bitmap_order, bitmap_pad;
  int bits_per_pixel, bytes_per_line, visual_class;
  int redmask, greenmask, bluemask;
  int bitsperrgb, numentriesincolormap, numcolorstructures;
  int windowwidth, windowheight, windowx, windowy, windowborderwidth;
  int numbytestowrite;
  int width, height;

  header_size = 104;
  fputintB(header_size, fp);

  file_version = 7;
  fputintB(file_version, fp);

  format = 2;  // ZPixmap
  fputintB(format, fp);

  depth = 24;
  fputintB(depth, fp);

  width = pxim->width;
  fputintB(width, fp);

  height = pxim->height;
  fputintB(height, fp);

  xoffset = 0;
  fputintB(xoffset, fp);

  byte_order = 0;
  fputintB(byte_order, fp);

  bitmap_unit = 32;
  fputintB(bitmap_unit, fp);

  bitmap_order = 0;
  fputintB(bitmap_order, fp);

  bitmap_pad = 32;
  fputintB(bitmap_pad, fp);

  bits_per_pixel = 32;
  fputintB(bits_per_pixel, fp);

  bytes_per_line = width * 4;
  fputintB(bytes_per_line, fp);

  visual_class = 5;  // more than one sample per pixel
  fputintB(visual_class, fp);

  redmask = 0x00ff0000;
  fputintB(redmask, fp);

  greenmask = 0x0000ff00;
  fputintB(greenmask, fp);

  bluemask = 0x000000ff;
  fputintB(bluemask, fp);

  // bitsperrgb = 8 ; // hard to believe
  bitsperrgb = 24;
  fputintB(bitsperrgb, fp);

  numentriesincolormap = 0;  // not used :
  fputintB(numentriesincolormap, fp);

  numcolorstructures = 0;
  fputintB(numcolorstructures, fp);

  windowwidth = width;
  fputintB(windowwidth, fp);

  windowheight = height;
  fputintB(windowheight, fp);

  windowx = 0;  // how crucial is this ?
  fputintB(windowx, fp);

  windowy = 0;  // how crucial is this ?
  fputintB(windowy, fp);

  windowborderwidth = 0;
  fputintB(windowborderwidth, fp);

  // null terminated window name fleshed out to multiple of 4 bytes
  fputc('\0', fp);
  fputc('\0', fp);
  fputc('\0', fp);
  fputc('\0', fp);

  numbytestowrite = bytes_per_line * height;

  fwrite(pxim->data, numbytestowrite, 1, fp);
}

void XImage_From_XWD_File(XImage* pxim, FILE* fp) {
  // assumes full color but uses full 32 bits per pixel

  int format, depth;
  int xoffset, byte_order, bitmap_unit, bitmap_order, bitmap_pad;
  int bits_per_pixel, bytes_per_line;

  int width, height;

  int numbytestoread;

  fgetintB(fp);
  fgetintB(fp);
  format = fgetintB(fp);
  depth = fgetintB(fp);
  width = fgetintB(fp);
  height = fgetintB(fp);
  xoffset = fgetintB(fp);
  byte_order = fgetintB(fp);
  bitmap_unit = fgetintB(fp);
  bitmap_order = fgetintB(fp);
  bitmap_pad = fgetintB(fp);
  bits_per_pixel = fgetintB(fp);
  bytes_per_line = fgetintB(fp);
  fgetintB(fp);
  fgetintB(fp);
  fgetintB(fp);
  fgetintB(fp);
  fgetintB(fp);
  fgetintB(fp);
  fgetintB(fp);
  fgetintB(fp);
  fgetintB(fp);
  fgetintB(fp);
  fgetintB(fp);
  fgetintB(fp);

  // null terminated window name fleshed out to multiple of 4 bytes
  fgetc(fp);
  fgetc(fp);
  fgetc(fp);
  fgetc(fp);

  /* ++++++++++++++++++++++++ */

  pxim->width = width;
  pxim->height = height;
  pxim->depth = depth;
  pxim->xoffset = xoffset;
  pxim->format = format;
  pxim->bitmap_unit = bitmap_unit;
  pxim->bitmap_pad = bitmap_pad;
  pxim->bytes_per_line = bytes_per_line;
  pxim->bits_per_pixel = bits_per_pixel;
  pxim->byte_order = byte_order;
  pxim->bitmap_bit_order = bitmap_order;

  /* on users head to have freed up any previous memory that pxim->data
    might have pointed to...trying to free it up here will cause faults
    if not already pointing to something sane */

  numbytestoread = pxim->bytes_per_line * pxim->height;
  pxim->data = (char*)malloc(numbytestoread);
  if (pxim->data == NULL) {
    printf("ERROR: XImage_From_XWD_File : can't malloc space needed\n");
    printf("Program terminating\n\n");
    exit(1);
  }

  fread(pxim->data, numbytestoread, 1, fp);

  rewind(fp);
}

int Save_Image_To_File_X(const void* filename)
// return 1 if successful else 0
{
  FILE* fp;
  XImage* pxim;

  fp = fopen((char*)filename, "w");
  if (fp == NULL) {
    printf("Save_Image_To_File_X cannot open file %s\n", (char*)filename);
    return 0;
  }

  //  pxim = XGetImage (XxDisplay, XxWindow,0,0, Xx_Pix_width, Xx_Pix_height) ;
  pxim = XGetImage(XxDisplay, XxDrawable, 0, 0, Xx_Pix_width, Xx_Pix_height,
                   AllPlanes, ZPixmap);

  XImage_To_XWD_File(pxim, fp);

  XDestroyImage(pxim);  // lack of this was causing mem leaks when
  // many images were being saved for movies

  fclose(fp);

  return 1;
}

int Get_Image_From_File_X(const void* filename, double Dx, double Dy)
// Put lower left corner of file into the graphics window at (x,y).
// return 1 if successful else 0
{
  int x = (int)Dx;
  int y = (int)Dy;

  FILE* fp;
  XImage xim[1];
  int srcx, srcy, destx, desty;
  int image_width, image_height;
  int transfer_width, transfer_height;

  fp = fopen((char*)filename, "r");
  if (fp == NULL) {
    printf("Get_Image_From_File_X cannot open file %s\n", (char*)filename);
    return 0;
  }

  XImage_From_XWD_File(&xim[0], fp);
  image_width = xim[0].width;
  image_height = xim[0].height;
  //  printf("%d %d\n",image_width,image_height) ;

  if (image_height <= Xx_Pix_height - y) {
    transfer_height = image_height;
    srcx = 0;
    srcy = 0;
    destx = x;
    desty = Xx_Pix_height - y - image_height;
    if (image_width <= Xx_Pix_width - x) {
      transfer_width = image_width;
    } else {
      transfer_width = Xx_Pix_width - x;
    }

  } else {
    transfer_height = Xx_Pix_height - y;
    srcx = 0;
    srcy = image_height - (Xx_Pix_height - y);
    destx = x;
    desty = 0;
    if (image_width <= Xx_Pix_width - x) {
      transfer_width = image_width;
    } else {
      transfer_width = Xx_Pix_width - x;
    }
  }

  XPutImage(XxDisplay, XxDrawable, XxPixmapContext, &xim[0], srcx, srcy, destx,
            desty, transfer_width, transfer_height);
  //	     Xx_Pix_width, Xx_Pix_height) ;

  fclose(fp);

  return 1;
}

int Get_Image_Dimensions_From_File_X(int d[2], void* filename)
// return 1 if successful else 0
// You do NOT need to have initialized the graphics environment
// before you call this function
{
  FILE* fp;

  fp = fopen((char*)filename, "r");
  if (fp == NULL) {
    printf("Get_Image_Dimensions_From_File_X cannot open file %s\n",
           (char*)filename);
    return 0;
  }

  // assumes full color but uses full 32 bits per pixel
  int width, height;

  fgetintB(fp);
  fgetintB(fp);
  fgetintB(fp);
  fgetintB(fp);
  width = fgetintB(fp);
  height = fgetintB(fp);

  d[0] = width;
  d[1] = height;

  fclose(fp);

  return 1;
}

int Get_Pixel_X(double Dx, double Dy)
// return the 32 bit pixel value...assumes x,y are legal
// i.e. it is NOT safe
{
  int x = (int)Dx;
  int y = (int)Dy;

  XImage* pxim;
  int p;

  pxim = XGetImage(XxDisplay, XxDrawable, 0, 0, Xx_Pix_width, Xx_Pix_height,
                   AllPlanes, ZPixmap);
  p = XGetPixel(pxim, x, Xx_Pix_height - 1 - y);

  XDestroyImage(pxim);  // lack of this was causing mem leaks when
  // many images were being saved for movies

  return p;
}

int Get_Pixel_SAFE_X(double Dx, double Dy, int pixel[1])
// return 1 if successful, else 0
{
  int x = (int)Dx;
  int y = (int)Dy;
  XImage* pxim;

  if ((x < 0) || (x >= Xx_Pix_width) || (y < 0) || (y >= Xx_Pix_height))
    return 0;

  pxim = XGetImage(XxDisplay, XxDrawable, 0, 0, Xx_Pix_width, Xx_Pix_height,
                   AllPlanes, ZPixmap);

  pixel[0] = XGetPixel(pxim, x, Xx_Pix_height - 1 - y);

  XDestroyImage(pxim);  // lack of this was causing mem leaks when
  // many images were being saved for movies

  return 1;
}

/////////////////////////////////////////////////////////////////
// suppose there were just 4 colors, 0,1,2,3
// [0.00, 0.25) -> 0
// [0.25, 0.50) -> 1
// [0.50, 0.75) -> 2
// [0.75, 1.00] -> 3
// 0 -> middle of [0.00, 0.25)
// 1 -> middle of [0.25, 0.50)
// 2 -> middle of [0.50, 0.75)
// 3 -> middle of [0.75, 1.00)

void Set_Color_Rgb_X(int r, int g, int b) {
  unsigned long int p;

  if (r < 0)
    r = 0;
  else if (r > 255)
    r = 255;
  if (g < 0)
    g = 0;
  else if (g > 255)
    g = 255;
  if (b < 0)
    b = 0;
  else if (b > 255)
    b = 255;

  p = (r << 16) | (g << 8) | (b);
  XSetForeground(XxDisplay, XxPixmapContext, p);

  // new
  Current_Red_Int = r;
  Current_Green_Int = g;
  Current_Blue_Int = b;
  Current_Color_Pixel = p;
}

void Set_Color_Rgb_DX(double dr, double dg, double db) {
  int r, g, b;

  if (dr < 0.0)
    dr = 0.0;
  else if (dr > 1.0)
    dr = 1.0;
  if (dg < 0.0)
    dg = 0.0;
  else if (dg > 1.0)
    dg = 1.0;
  if (db < 0.0)
    db = 0.0;
  else if (db > 1.0)
    db = 1.0;

  r = (int)(256 * dr);
  g = (int)(256 * dg);
  b = (int)(256 * db);

  Set_Color_Rgb_X(r, g, b);
}

void Convert_Pixel_To_rgbI_X(int pixel, int rgbI[3]) {
  rgbI[2] = pixel & 0xff;
  rgbI[1] = (pixel >> 8) & 0xff;
  rgbI[0] = (pixel >> 16) & 0xff;
}

void Convert_rgbI_To_rgb_X(int rgbI[3], double rgb[3])
// convert rgbI[] in 0-255 to rgb[] in 0.0 - 1.0
// return 1 if successful, else 0
{
  double d;
  d = 0.5 / 256.0;
  // see above discussion of middle of intervals for purpose of d
  rgb[0] = rgbI[0] / 256.0 + d;
  rgb[1] = rgbI[1] / 256.0 + d;
  rgb[2] = rgbI[2] / 256.0 + d;
}

void XImage_to_Display(XImage* pxim, double Dx, double Dy)
// Put lower left corner of file into the graphics window at (x,y).
// return 1 if successful else 0
{
  int x = (int)Dx;
  int y = (int)Dy;

  int image_width, image_height;
  int srcx, srcy, destx, desty;
  int transfer_width, transfer_height;

  image_width = pxim->width;
  image_height = pxim->height;

  if (image_height <= Xx_Pix_height - y) {
    transfer_height = image_height;
    srcx = 0;
    srcy = 0;
    destx = x;
    desty = Xx_Pix_height - y - image_height;
    if (image_width <= Xx_Pix_width - x) {
      transfer_width = image_width;
    } else {
      transfer_width = Xx_Pix_width - x;
    }

  } else {
    transfer_height = Xx_Pix_height - y;
    srcx = 0;
    srcy = image_height - (Xx_Pix_height - y);
    destx = x;
    desty = 0;
    if (image_width <= Xx_Pix_width - x) {
      transfer_width = image_width;
    } else {
      transfer_width = Xx_Pix_width - x;
    }
  }

  XPutImage(XxDisplay, XxDrawable, XxPixmapContext, pxim, srcx, srcy, destx,
            desty, transfer_width, transfer_height);
  //	     Xx_Pix_width, Xx_Pix_height) ;
}

XImagePointer Get_ximage_of_display()
// should be used with caution...
// XImage pxim = Get_ximage_of_display() ;
// later on, should do
// XDestroyImage(pxim)
// when done with ximage or could create memory leak
{
  XImage* pxim;

  pxim = XGetImage(XxDisplay, XxDrawable, 0, 0, Xx_Pix_width, Xx_Pix_height,
                   AllPlanes, ZPixmap);

  return pxim;
}

//====================================================================
// G stuff :

/////////////////////////////////////////////////////////////////////
// the G_ routines are graphics related and should
// not be called unless G_init_graphics has been called first
/////////////////////////////////////////////////////////////////////
// Xx following are merely pointers
// which are initialized by G_Init
// Xx actual space for the pointers is declared here
// Xx header file has these pointers declared as "extern"
/////////////////////////////////////////////////////////////////////

void (*G_close)();
// terminate the graphics

void (*G_display_image)();
// make drawing visible if it already isn't
// in some environments, this might not do anything
// in others it might copy a buffer and/or do other
// synchronizing tasks

int (*Gi_events)(int* d);

int (*G_events)(double* d);

int (*G_resize_window)(int request[2], int actual[2]);
// request[0] = desired width
// request[1] = desired heigth
// actual[0] = width you actually got
// actual[1] = height you actuall got
// added jan 23, 2014

void (*Gi_get_current_window_dimensions)(int* dimentsions);
// return 1 if successful
// needs to be passed an array of two ints

void (*G_get_current_window_dimensions)(double* dimentsions);
// return 1 if successful
// needs to be passed an array of two doubles

void (*Gi_rgb)(int r, int g, int b);
// assumes r,g,b are ints in [0,255]

void (*G_rgb)(double r, double g, double b);
// assumes r,g,b are doubles in [0, 1]

/////////////////////////////////////////////////////////////////////
// Xx next batch are based on the ability to repeatedly
//  plot a single pixel
/////////////////////////////////////////////////////////////////////

void (*G_pixel)(double x, double y);
// return 1 always
// This is not guaranteed to be safe

int (*G_point)(double x, double y);
// return 1 always
// This is SAFE.
// Draws a thick point whose dimensions are controlled by
// int G_change_pen_dimensions (double w, double h).

void (*G_circle)(double a, double b, double r);
// always return 1
// capable of drawing a circle with thick outline

void (*G_unclipped_line)(double ixs, double iys, double ixe, double iye);
// return 1 always
// This is SAFE.
// Capable of drawing a thick line.
// This is safe because it depends on G_point (which is SAFE)
// but because it is unclipped, this code could waste a great
// deal of time trying to plot lots of points outside the window.

void (*G_line)(double ixs, double iys, double ixe, double iye);
// return 0 if line clipped away entirely, else return 1
// This is SAFE.
// Capable of drawing a thick line.
// This also clips
// Note that the clipping occurs only for a thin, "perfect"
// line, not for a possibly thick line, but the safety feature
// of G_point that will actually plot a thick point, keeps the
// entire code safe.

int (*Gi_polygon)(int* x, int* y, int numpts);
// return 1 always
// capable of drawing a polygon with thick outline

int (*G_polygon)(double* x, double* y, double numpts);
// provided as an alternative for passing arrays of doubles
// return 1 always
// capable of drawing a polygon with thick outline

void (*G_triangle)(double x0, double y0, double x1, double y1, double x2,
                   double y2);
// return value it inherits from G_polygon
// capable of drawing a triangle with thick outline

void (*G_rectangle)(double xleft, double yleft, double width, double height);
// return value it inherits from G_polygon
// capable of drawing a rectangle with thick outline

/////////////////////////////////////////////////////////////////////
// This batch are based on the ability to repeatedly
// draw single pixel horizontal lines
/////////////////////////////////////////////////////////////////////

int (*G_single_pixel_horizontal_line)(double x0, double x1, double y);
// This is not guaranteed to be safe

void (*G_clear)();
// return 1 always

void (*G_fill_circle)(double a, double b, double r);
// always return 1

int (*G_unclipped_fill_polygon)(double* xx, double* yy, double n);
// return 0 if size needs to be truncated (unusual), else 1
// This is SAFE

int (*Gi_fill_polygon)(int* xx, int* yy, int n);
// return 0 if size needs to be truncated (unusual),
// or if clipping is used,  otherwise return 1
// This is SAFE
// AND it clips

int (*G_fill_polygon)(double* xx, double* yy, double n);
// provided as an alternative for passing arrays of doubles
// return 0 if size needs to be truncated (unusual),
// or if clipping is used,  otherwise return 1
// This is SAFE
// AND it clips

void (*G_fill_triangle)(double x0, double y0, double x1, double y1, double x2,
                        double y2);
// return value it inherits from G_fill_polygon

void (*G_fill_rectangle)(double xleft, double yleft, double width,
                         double height);
// return value it inherits from G_fill_polygon

int (*G_font_pixel_height)();
// return the font height in pixels

// the void * pointers below allow for passing in
// either an array of characters (usual usage)
// but also, perhaps, an double array of packed
// characters

int (*G_string_pixel_width)(const void* s);
// return the length in pixles of the string s

void (*G_draw_string)(const void* one_line_of_text, double LLx, double LLy);
// draw a single line of text beginning at (LLx,LLy) which specifies
// the coordinates of the lower left corner of the bounding box
// of the text

int (*G_save_image_to_file)(const void* filename);
// return 1 if successful, else 0

int (*G_get_image_from_file)(const void* filename, double x, double y);
// return 1 if successful, else 0

int (*G_get_pixel)(double x, double y);
// return the 32 bit pixel value...assumes x,y are legal
// i.e. it is NOT safe

int (*G_get_pixel_SAFE)(double x, double y, int pixel[1]);
// return 1 if successful, else 0

void (*G_convert_pixel_to_rgbI)(int pixel, int rgbI[3]);
// rgbI[] values in 0-255

void (*G_convert_rgbI_to_rgb)(int rgbI[3], double rgb[3]);
// convert rgbI[] in 0-255 to rgb[] in 0.0 - 1.0

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// Below are actual functions
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

int G_init_graphics(double w, double h)
// G_init_graphics has the task of connecting this interface
// with actual routines that can do the work in a
// specific environment, e.g. X11 or Windows or whatever
{
  int s;

  G_close = Close_Down_X;

  G_display_image = Copy_Buffer_And_Flush_X;

  Gi_events = Get_Events_X;

  G_events = Get_Events_DX;

  G_resize_window = Resize_window_X;

  Gi_get_current_window_dimensions = Get_Current_Dimensions_X;

  G_get_current_window_dimensions = Get_Current_Dimensions_DX;

  Gi_rgb = Set_Color_Rgb_X;

  G_rgb = Set_Color_Rgb_DX;

  G_pixel = Point_X;

  G_point = Safe_Point_X;

  G_circle = Circle_X;

  G_unclipped_line = Line_X;

  // G_line = Line_X ;
  G_line = Safe_Line_X;

  Gi_polygon = Polygon_X;

  G_polygon = Polygon_DX;

  G_triangle = Triangle_X;

  G_rectangle = Rectangle_X;

  G_single_pixel_horizontal_line = Horizontal_Single_Pixel_Line_X;

  G_clear = Clear_Buffer_X;

  G_fill_circle = Fill_Circle_X;

  G_unclipped_fill_polygon = Fill_Polygon_DX;

  Gi_fill_polygon = Fill_Polygon_X;

  G_fill_polygon = Fill_Polygon_DX;

  G_fill_triangle = Fill_Triangle_X;

  G_fill_rectangle = Fill_Rectangle_X;

  G_font_pixel_height = Font_Pixel_Height_X;

  G_string_pixel_width = String_Pixel_Width_X;

  G_draw_string = Draw_String_X;

  G_save_image_to_file = Save_Image_To_File_X;

  G_get_image_from_file = Get_Image_From_File_X;

  G_get_pixel = Get_Pixel_X;

  G_get_pixel_SAFE = Get_Pixel_SAFE_X;

  G_convert_pixel_to_rgbI = Convert_Pixel_To_rgbI_X;

  G_convert_rgbI_to_rgb = Convert_rgbI_To_rgb_X;

  s = Init_X(w, h);

  return s;
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

// The following functions are built on top of G
// not directly on top of the underlying
// video system

int Gi_wait_click(int p[2]) {
  int sig;

  G_display_image();
  do {
    sig = Gi_events(p);
  } while (sig != -3);

  return sig;
}

int G_wait_click(double p[2]) {
  int sig;
  int pi[2];

  sig = Gi_wait_click(pi);
  p[0] = pi[0];
  p[1] = pi[1];

  return sig;
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

int G_no_wait_key()
// if key has been hit, return non-negative (ascii) value
// return negative if key has NOT been hit  (some other action or no action)
{
  int p[2];
  int sig;
  sig = Gi_events(p);
  return sig;
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// Added Aug 28, 2012

static int sector(double xcenter, double ycenter, double radius,
                  double start_radians, double end_radians, double* x,
                  double* y, int num_pts_in_full_circle) {
  int num, j;
  double delta;

  delta = end_radians - start_radians;
  if (delta < 0) return 0;
  if (delta > 2 * M_PI) return 0;

  num = (int)(num_pts_in_full_circle * delta / (2 * M_PI));

  if (num < 1) {
    num = 1;
  } else if (num > 500) {
    num = 500;
  }

  j = 0;
  while (j <= num) {
    double theta = start_radians + j * delta / num;

    x[j] = xcenter + radius * cos(theta);
    y[j] = ycenter + radius * sin(theta);

    j++;
  }
  x[j] = xcenter;
  y[j] = ycenter;
  j++;

  return j;
}

int G_sector(double xcenter, double ycenter, double radius,
             double start_radians, double end_radians) {
  int num_pts_in_full_circle, n;
  double x[500 + 3], y[500 + 3];

  num_pts_in_full_circle = 500;

  n = sector(xcenter, ycenter, radius, start_radians, end_radians, x, y,
             num_pts_in_full_circle);

  // normally, n == num_pts_in_full_circle + 1

  if (n <= 0) {
    printf("Sector flaw\n");
  } else {
    G_polygon(x, y, n);
  }

  return 1;
}

int G_fill_sector(double xcenter, double ycenter, double radius,
                  double start_radians, double end_radians) {
  int num_pts_in_full_circle, n;
  double x[500 + 3], y[500 + 3];

  num_pts_in_full_circle = 500;

  n = sector(xcenter, ycenter, radius, start_radians, end_radians, x, y,
             num_pts_in_full_circle);

  // normally, n == num_pts_in_full_circle + 1

  if (n <= 0) {
    printf("Sector flaw\n");
  } else {
    G_fill_polygon(x, y, n);
  }

  return 1;
}

/////////////////////////////////////////////////////////////////

static double mouse_values[2];

void G_wait_mouse()
// save off mouse click in reserved variables
{
  G_wait_click(mouse_values);
}

double G_x_mouse()
// return x coordinate of last G_wait_mouse
{
  return mouse_values[0];
}

double G_y_mouse()
// return y coordinate of last G_wait_mouse
{
  return mouse_values[1];
}

//====================================================================
// Time :

int get_timeI(int* hms)
// hms had better be at least 3 ints long
// returns 1 for success, 0 for failure
// which is the opposite of what the standard
// gettimeofday returns
{
  int failure;
  struct timeval Tp, *tp;
  struct timezone Tzp, *tzp;
  time_t numseconds, *clock;
  struct tm* x;

  tp = &Tp;
  tzp = &Tzp;
  failure = gettimeofday(tp, tzp);  // 1 for fail, 0 for success
  numseconds = tp->tv_sec;
  clock = &numseconds;

  x = localtime(clock);

  hms[0] = x->tm_hour;
  hms[1] = x->tm_min;
  hms[2] = x->tm_sec;

  return (1 - failure);
}

int get_timeD(double* hms)
// hms had better be at least 3 doubles long
// returns 1 for success, 0 for failure
// which is the opposite of what the standard
// gettimeofday returns
{
  int ihms[3];
  int s;

  s = get_timeI(ihms);

  hms[0] = ihms[0];
  hms[1] = ihms[1];
  hms[2] = ihms[2];

  return s;
}

void get_time(double* hms)
// synonym for get_timeD
{
  get_timeD(hms);
}

void G_get_time(double* hms) {
  // another synonym for get_timeD
  get_timeD(hms);
}

#endif
