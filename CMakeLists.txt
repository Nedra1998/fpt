cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(FPT C)

option(EXAMPLE "Build example program" OFF)

set(CMAKE_C_STANDARD 11)

find_package(SDL)
if(SDL_FOUND)
  message("FOUND SDL")
  add_definitions(-DFPT_SDL)
endif()

find_package(X11)
if(X11_FOUND)
  message("FOUND X11")
  add_definitions(-DFPT_X11)
endif()

file(GLOB_RECURSE fpt_src ${CMAKE_CURRENT_SOURCE_DIR} "src/*.c")
add_library(fpt ${fpt_src})

if(EXAMPLE)
  file(GLOB_RECURSE exe_src "${CMAKE_CURRENT_SOURCE_DIR}" "example/*.c")
  add_executable(fpt.exe ${exe_src})
  target_include_directories(fpt.exe PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
  target_link_libraries(fpt.exe fpt)
endif()
