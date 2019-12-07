#include "fpt/fpt.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl.h"

#define EVENT_QUEUE_SIZE 64

typedef struct glfw_event {
  int key;
  float px, py;
} glfw_event_t;

static GLFWwindow* window_ = NULL;
static size_t width_ = 0, height_ = 0;
static float r_ = 0.0, g_ = 0.0, b_ = 0.0, pen_size_ = 1.0;
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
    push_event((glfw_event_t){-button - 1, (float)px, (float)py});
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
  glfwSwapInterval(1);
  glViewport(0, 0, width_, height_);
  glOrtho(0, width_, 0, height_, -1, 1);
  glfwSetFramebufferSizeCallback(window_, glfw_framebuffer_size_callback);
  glfwSetCharCallback(window_, glfw_char_callback);
  glfwSetMouseButtonCallback(window_, glfw_mouse_button_callback);

  return true;
}

bool G_close() {
  if (window_ != NULL) return true;
  glfwDestroyWindow(window_);
  glfwTerminate();
  return true;
}

void G_display_image() { glfwSwapBuffers(window_); }

void G_clear() { glClear(GL_COLOR_BUFFER_BIT); }

void G_rgb(float red, float green, float blue) {
  r_ = red > 0.0f ? red < 1.0f ? red : 1.0f : 0.0f;
  g_ = green > 0.0f ? green < 1.0f ? green : 1.0f : 0.0f;
  b_ = blue > 0.0f ? blue < 1.0f ? blue : 1.0f : 0.0f;
  glClearColor(r_, g_, b_, 0.0f);
  glColor3f(r_, g_, b_);
}
void G_set_pen_size(float size) {
  pen_size_ = size;
  glPointSize(size);
}

void G_point(float x, float y) {
  glBegin(GL_POINTS);
  glVertex2f(x, height_ - 1 - y);
  glEnd();
}
void G_line(float sx, float sy, float ex, float ey) {
  glBegin(GL_LINES);
  glVertex2f(sx, height_ - 1 - sy);
  glVertex2f(ex, height_ - 1 - ey);
  glEnd();
}
void G_rectangle(float x, float y, float w, float h) {
  glBegin(GL_LINE_LOOP);
  glVertex2f(x, height_ - 1 - y - h);
  glVertex2f(x + w, height_ - 1 - y - h);
  glVertex2f(x + w, height_ - 1 - y);
  glVertex2f(x, height_ - 1 - y);
  glEnd();
}
void G_fill_rectangle(float x, float y, float w, float h) {
  glBegin(GL_QUADS);
  glVertex2f(x, height_ - 1 - y - h);
  glVertex2f(x + w, height_ - 1 - y - h);
  glVertex2f(x + w, height_ - 1 - y);
  glVertex2f(x, height_ - 1 - y);
  glEnd();
}
void G_triangle(float x1, float y1, float x2, float y2, float x3, float y3) {
  glBegin(GL_LINE_LOOP);
  glVertex2f(x1, height_ - 1 - y1);
  glVertex2f(x2, height_ - 1 - y2);
  glVertex2f(x3, height_ - 1 - y3);
  glEnd();
}
void G_fill_triangle(float x1, float y1, float x2, float y2, float x3,
                     float y3) {
  glBegin(GL_TRIANGLES);
  glVertex2f(x1, height_ - 1 - y1);
  glVertex2f(x2, height_ - 1 - y2);
  glVertex2f(x3, height_ - 1 - y3);
  glEnd();
}
int G_handle_events(float* px, float* py) {
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
int Gi_events(float* pos) { return G_handle_events(&pos[0], &pos[1]); }
int G_wait_key() {
  float pos[2];
  int key;
  G_display_image();
  do {
    key = Gi_events(pos);
  } while (key <= 0);
  return key;
}
void Gi_get_current_window_dimensions(size_t* dim) {
  dim[0] = width_;
  dim[1] = height_;
}
