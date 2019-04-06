cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(FPT C)

option(EXAMPLE "Build example program" ON)
option(SDL "Force using SDL2" OFF)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE fpt_hdr ${CMAKE_CURRENT_SOURCE_DIR} "src/*.h")
file(GLOB_RECURSE fpt_src ${CMAKE_CURRENT_SOURCE_DIR} "src/*.c")
add_library(fpt ${fpt_src})

find_package(X11)
if(X11_FOUND AND NOT SDL)
  add_definitions(-DFPT_X11)
  target_include_directories(fpt PUBLIC ${X11_INCLUDE_DIR})
  target_link_libraries(fpt ${X11_LIBRARIES})
else()
  message("USING SDL2")
  add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/sdl")
  target_include_directories(fpt PUBLIC ${SDL2_INCLUDE_DIR})
  target_link_libraries(fpt ${SDL2_LIBRARIES})
endif()

install(TARGETS fpt LIBRARY DESTINATION lib ARCHIVE DESTINATION lib/static)
install(FILES ${fpt_hdr} DESTINATION include/fpt)

if(NOT TARGET uninstall)
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/uninstall.cmake" IMMEDIATE @ONLY)
  add_custom_target(uninstall "${CMAKE_COMMAND}" -P
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/uninstall.cmake")
endif()

if(EXAMPLE)
  file(GLOB_RECURSE exe_src "${CMAKE_CURRENT_SOURCE_DIR}" "example/*.c")
  add_executable(fpt.exe ${exe_src})
  target_include_directories(fpt.exe PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
  target_link_libraries(fpt.exe fpt m)
endif()
