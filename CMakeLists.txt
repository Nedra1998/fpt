CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)
PROJECT(FPT C)

OPTION(EXAMPLE "Build example program" ON)
OPTION(SDL "Force using SDL2" OFF)

SET(CMAKE_C_STANDARD 11)
SET(CMAKE_C_COMPILER clang)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

FILE(GLOB_RECURSE HEADERS_FILES ${CMAKE_CURRENT_SOURCE_DIR} "src/*.h")
FILE(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR} "src/*.c")
ADD_LIBRARY(FPT ${SOURCE_FILES})

SET(FPT_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src")
SET(FPT_LIBRARIES FPT)
SET(FPT_COMPILE_LIBRARIES)

FIND_PACKAGE(X11)
if(X11_FOUND AND NOT SDL)
  ADD_DEFINITIONS(-DFPT_X11)
  LIST(APPEND FPT_INCLUDE_DIRS ${X11_INCLUDE_DIR})
  LIST(APPEND FPT_LIBRARIES ${X11_LIBRARIES})
  LIST(APPEND FPT_COMPILE_LIBRARIES ${X11_LIBRARIES})
elseif(SDL)
  FIND_PACKAGE(SDL2)
  add_definitions(-DFPT_SDL)
  LIST(APPEND FPT_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS})
  LIST(APPEND FPT_LIBRARIES ${SDL2_LIBRARIES})
  LIST(APPEND FPT_COMPILE_LIBRARIES ${SDL2_LIBRARIES})
  find_package(SDL_image)
  if(SDL_IMAGE_FOUND)
    add_definitions(-DFPT_SDL_IMG)
    LIST(APPEND FPT_INCLUDE_DIRS ${SDL_IMAGE_INCLUDE_DIRS})
    LIST(APPEND FPT_LIBRARIES ${SDL_IMAGE_LIBRARIES})
    LIST(APPEND FPT_COMPILE_LIBRARIES ${SDL_IMAGE_LIBRARIES})
  endif()
  find_package(SDL_ttf)
  if(SDL_TTF_FOUND)
    add_definitions(-DFPT_SDL_TTF)
    LIST(APPEND FPT_INCLUDE_DIRS ${SDL_TTF_INCLUDE_DIRS})
    LIST(APPEND FPT_LIBRARIES ${SDL_TTF_LIBRARIES})
    LIST(APPEND FPT_COMPILE_LIBRARIES ${SDL_TTF_LIBRARIES})
  endif()
else()
  message(FATAL_ERROR "\nX11/XQUARTZ or SDL2 are required for the installiation of FPT. Please us the provided install.sh srcipt to properly install all of the required packages for your operating system.\nOptionally you may also install SDL2 manualy, and use that (Although it is not recomended for preformance reasons).\nIf this message has appeared while using the install.sh script, please create an issue here: https://github.com/ardenrasmussen/fpt/issues. Please include your operating system, and if applicable the distro and version.")
endif()

TARGET_INCLUDE_DIRECTORIES(FPT PUBLIC ${FPT_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(FPT ${FPT_COMPILE_LIBRARIES})

INSTALL(TARGETS FPT LIBRARY DESTINATION lib ARCHIVE DESTINATION lib/static)
INSTALL(FILES ${HEADERS_FILES} DESTINATION include/fpt)

if(NOT TARGET uninstall)
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/uninstall.cmake" IMMEDIATE @ONLY)
  add_custom_target(uninstall "${CMAKE_COMMAND}" -P
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/uninstall.cmake")
endif()

ADD_SUBDIRECTORY("examples")

IF (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json")
  EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
    ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    )
ENDIF()
