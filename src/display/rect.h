#ifndef RECT_H_
#define RECT_H_
#include "SDL_sysrender.h"
namespace sdl
{
bool Intersection(SDL_FRect const* a, SDL_FRect const* b);
bool Intersection(SDL_FRect const* a, SDL_FRect const* b, SDL_FRect* ret);
}
#endif