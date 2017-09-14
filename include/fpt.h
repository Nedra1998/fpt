/*
 * The freshman programming toolkit for Lewis & Clark College.
 *
 * Created by Arden Rasmussen 2017
 *
 * Documentation is available in at https://github.com/ardenrasmussen/fpt or
 * can be found int the documentation folder of source.
 */

#if __linux__
#define SDL
#elif __unix__
#define SDL
#elif __APPLE__
#define SDL
#elif __WIN32
#define SDL
#endif

#include "graphics.h"
#include "input.h"
