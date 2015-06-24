#ifndef RECT_H_
#define RECT_H_
#include "SDL_sysrender.h"
namespace sdl
{
auto Intersection(SDL_FRect const* a, SDL_FRect const* b) -> bool;
auto Intersection(SDL_FRect const* a, SDL_FRect const* b, SDL_FRect* ret) -> bool;
}
#endif