cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(FPT C)

option(EXAMPLE "Build example program" ON)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE fpt_src ${CMAKE_CURRENT_SOURCE_DIR} "src/*.c")
add_library(fpt ${fpt_src})
target_link_libraries(fpt m)

find_package(SDL)
if(SDL_FOUND)
  add_definitions(-DFPT_SDL)
endif()

find_package(X11)
if(X11_FOUND)
  add_definitions(-DFPT_X11)
  target_include_directories(fpt PUBLIC ${X11_INCLUDE_DIR})
  target_link_libraries(fpt ${X11_LIBRARIES})
endif()

if(EXAMPLE)
  file(GLOB_RECURSE exe_src "${CMAKE_CURRENT_SOURCE_DIR}" "example/*.c")
  add_executable(fpt.exe ${exe_src})
  target_include_directories(fpt.exe PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
  target_link_libraries(fpt.exe fpt m)
endif()
