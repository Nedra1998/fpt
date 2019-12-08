#include "fpt/fpt.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl.h"

#define EVENT_QUEUE_SIZE 64

typedef struct glfw_event {
  int key;
  double px, py;
} glfw_event_t;

static GLFWwindow* window_ = NULL;
static size_t width_ = 0, height_ = 0;
static double r_ = 0.0, g_ = 0.0, b_ = 0.0, pen_size_ = 1.0;
static glfw_event_t event_queue_[EVENT_QUEUE_SIZE];
static size_t event_index_ = 0;

void push_event(glfw_event_t ev) {
  if (event_index_ == EVENT_QUEUE_SIZE) {
    for (size_t i = 0; i < event_index_ - 1; ++i) {
      event_queue_[i] = event_queue_[i + 1];
    }
    event_index_--;
  }
  event_queue_[event_index_] = ev;
  event_index_++;
}
void pop_event(glfw_event_t* ev) {
  if (event_index_ == 0) {
    ev = NULL;
    return;
  } else {
    *ev = event_queue_[0];
    for (size_t i = 0; i < event_index_ - 1; ++i) {
      event_queue_[i] = event_queue_[i + 1];
    }
    event_index_--;
  }
}

void glfw_error_callback(int error_code, const char* description) {
  fprintf(stderr, "GLFW [%d]: %s", error_code, description);
}
void glfw_framebuffer_size_callback(GLFWwindow* win, int w, int h) {
  width_ = w;
  height_ = h;
  glViewport(0, 0, width_, height_);
}
void glfw_char_callback(GLFWwindow* win, unsigned int codepoint) {
  push_event((glfw_event_t){(int)codepoint, 0.0f, 0.0f});
}
void glfw_mouse_button_callback(GLFWwindow* win, int button, int action,
                                int mods) {
  if (action == GLFW_RELEASE) {
    double px, py;
    glfwGetCursorPos(window_, &px, &py);
    push_event((glfw_event_t){-button - 1, (double)px, (double)py});
  }
}

bool G_init_graphics(size_t w, size_t h) {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    const char* glfw_err;
    glfwGetError(&glfw_err);
    fprintf(stderr, "Failed to initalize GLFW: %s\n", glfw_err);
    return false;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);

  int glfw_major, glfw_minor, glfw_patch;
  glfwGetVersion(&glfw_major, &glfw_minor, &glfw_patch);
  width_ = w;
  height_ = h;
  window_ = glfwCreateWindow(width_, height_, "FPT", NULL, NULL);
  if (window_ == NULL) {
    fprintf(stderr, "Failed to create GLFW window\n");
    glfwTerminate();
    return false;
  }
  glfwMakeContextCurrent(window_);
  if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
    fprintf(stderr, "Failed to load GL with GLAD\n");
    glfwDestroyWindow(window_);
    glfwTerminate();
    return false;
  };
  glViewport(0, 0, width_, height_);
  glOrtho(0, width_, 0, height_, -1, 1);
  glfwSetFramebufferSizeCallback(window_, glfw_framebuffer_size_callback);
  glfwSetCharCallback(window_, glfw_char_callback);
  glfwSetMouseButtonCallback(window_, glfw_mouse_button_callback);
  glFlush();

  return true;
}

bool G_close() {
  if (window_ != NULL) return true;
  glfwDestroyWindow(window_);
  glfwTerminate();
  return true;
}

void G_display_image() { glFlush(); }

void G_clear() { glClear(GL_COLOR_BUFFER_BIT); }

void G_rgb(double red, double green, double blue) {
  r_ = red > 0.0f ? red < 1.0f ? red : 1.0f : 0.0f;
  g_ = green > 0.0f ? green < 1.0f ? green : 1.0f : 0.0f;
  b_ = blue > 0.0f ? blue < 1.0f ? blue : 1.0f : 0.0f;
  glClearColor(r_, g_, b_, 0.0f);
  glColor3f(r_, g_, b_);
}
void G_set_pen_size(double size) {
  pen_size_ = size;
  glPointSize(size);
}

void G_point(double x, double y) {
  glBegin(GL_POINTS);
  glVertex2f(x, height_ - 1 - y);
  glEnd();
}
void G_pixel(double x, double y) {
  glBegin(GL_POINTS);
  glVertex2f(x, height_ - 1 - y);
  glEnd();
}
void G_line(double sx, double sy, double ex, double ey) {
  glBegin(GL_LINES);
  glVertex2f(sx, height_ - 1 - sy);
  glVertex2f(ex, height_ - 1 - ey);
  glEnd();
}
void G_rectangle(double x, double y, double w, double h) {
  glBegin(GL_LINE_LOOP);
  glVertex2f(x, height_ - 1 - y - h);
  glVertex2f(x + w, height_ - 1 - y - h);
  glVertex2f(x + w, height_ - 1 - y);
  glVertex2f(x, height_ - 1 - y);
  glEnd();
}
void G_fill_rectangle(double x, double y, double w, double h) {
  glBegin(GL_QUADS);
  glVertex2f(x, height_ - 1 - y - h);
  glVertex2f(x + w, height_ - 1 - y - h);
  glVertex2f(x + w, height_ - 1 - y);
  glVertex2f(x, height_ - 1 - y);
  glEnd();
}
void G_triangle(double x1, double y1, double x2, double y2, double x3,
                double y3) {
  glBegin(GL_LINE_LOOP);
  glVertex2f(x1, height_ - 1 - y1);
  glVertex2f(x2, height_ - 1 - y2);
  glVertex2f(x3, height_ - 1 - y3);
  glEnd();
}
void G_fill_triangle(double x1, double y1, double x2, double y2, double x3,
                     double y3) {
  glBegin(GL_TRIANGLES);
  glVertex2f(x1, height_ - 1 - y1);
  glVertex2f(x2, height_ - 1 - y2);
  glVertex2f(x3, height_ - 1 - y3);
  glEnd();
}
void G_polygon(double* x, double* y, size_t n) {
  glBegin(GL_LINE_LOOP);
  for (size_t i = 0; i < n; ++i) {
    glVertex2f(x[i], height_ - 1 - y[i]);
  }
  glEnd();
}
void G_fill_polygon(double* x, double* y, size_t n) {
  glBegin(GL_POLYGON);
  for (size_t i = 0; i < n; ++i) {
    glVertex2f(x[i], height_ - 1 - y[i]);
  }
  glEnd();
}
void G_circle(double cx, double cy, double r) {
  glBegin(GL_LINE_LOOP);
  for (double theta = 0.0f; theta < 2.0f * M_PI;
       theta += 2.0f * M_PI / (r + 10.0)) {
    glVertex2f(r * sin(theta) + cx, height_ - 1 - (r * cos(theta) + cy));
  }
  glEnd();
}
void G_fill_circle(double cx, double cy, double r) {
  glBegin(GL_POLYGON);
  for (double theta = 0.0f; theta < 2.0f * M_PI;
       theta += 2.0f * M_PI / (r + 10.0)) {
    glVertex2f(r * sin(theta) + cx, height_ - 1 - (r * cos(theta) + cy));
  }
  glEnd();
}

int G_handle_events(double* px, double* py) {
  *px = 0.0f;
  *py = 0.0f;
  glfwPollEvents();
  while (event_index_ != 0) {
    glfw_event_t ev;
    pop_event(&ev);
    if (ev.key < 0) {
      *px = ev.px;
      *py = ev.py;
      return ev.key;
    } else if (ev.key > 0) {
      return ev.key;
    }
  }
  return 0;
}
int G_events(double* pos) { return G_handle_events(&pos[0], &pos[1]); }
int G_wait_event(double pos[2]) {
  int key;
  G_display_image();
  do {
    key = G_events(pos);
  } while (key == 0);
  return key;
}
int G_wait_key() {
  double pos[2];
  int key;
  G_display_image();
  do {
    key = G_events(pos);
  } while (key <= 0);
  return key;
}
int G_wait_click(double pos[2]) {
  int button;
  G_display_image();
  do {
    button = G_events(pos);
  } while (button >= 0);
  return -button - 1;
}
void Gi_get_current_window_dimensions(size_t* dim) {
  dim[0] = width_;
  dim[1] = height_;
}
bool G_save_image_to_file(const char* path) {
  FILE* out;
  int filesize = 54 + 3 * width_ * height_;
  unsigned char* img =
      (unsigned char*)malloc(sizeof(unsigned char) * 3 * width_ * height_);
  glReadPixels(0, 0, width_, height_, GL_BGR, GL_UNSIGNED_BYTE, img);
  unsigned char bmpfileheader[14] = {'B', 'M', 0, 0,  0, 0, 0,
                                     0,   0,   0, 54, 0, 0, 0};
  unsigned char bmpinfoheader[40] = {40, 0, 0, 0, 0, 0, 0,  0,
                                     0,  0, 0, 0, 1, 0, 24, 0};
  unsigned char bmppad[3] = {0, 0, 0};

  bmpfileheader[2] = (unsigned char)(filesize);
  bmpfileheader[3] = (unsigned char)(filesize >> 8);
  bmpfileheader[4] = (unsigned char)(filesize >> 16);
  bmpfileheader[5] = (unsigned char)(filesize >> 24);

  bmpinfoheader[4] = (unsigned char)(width_);
  bmpinfoheader[5] = (unsigned char)(width_ >> 8);
  bmpinfoheader[6] = (unsigned char)(width_ >> 16);
  bmpinfoheader[7] = (unsigned char)(width_ >> 24);
  bmpinfoheader[8] = (unsigned char)(height_);
  bmpinfoheader[9] = (unsigned char)(height_ >> 8);
  bmpinfoheader[10] = (unsigned char)(height_ >> 16);
  bmpinfoheader[11] = (unsigned char)(height_ >> 24);

  out = fopen(path, "wb");
  fwrite(bmpfileheader, 1, 14, out);
  fwrite(bmpinfoheader, 1, 40, out);
  for (size_t i = height_; i > 0; --i) {
    fwrite(img + (width_ * (height_ - i) * 3), 3, width_, out);
    fwrite(bmppad, 1, (4 - (width_ * 3) % 4) % 4, out);
  }
  free(img);
  fclose(out);
  return true;
}
