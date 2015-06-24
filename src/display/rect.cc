#include "rect.h"
#include <utility>
#include <algorithm>
namespace
{
auto Intersects(float l0, float w0, float l1, float w1, float* l_out, float* w_out) -> bool
{
  float r0 = l0 + w0;
  if(l0 > r0)
  {
    std::swap(l0, r0);
  }
  float r1 = l1 + w1;
  if(l1 > r1)
  {
    std::swap(l1, r1);
  }
  if(l0 > l1)
  {
    std::swap(l1, l0);
    std::swap(r1, r0);
  }

  bool intersection = (l1 <= r0);
  if(intersection)
  {
    if(l_out || w_out)
    { 
      float l = l1;
      float w = std::min(r0, r1) - l1;
      if((w0 < 0.f) ^ (w1 < 0.f))
      {
        l += w;
        w *= -1.f;
      }
      if(l_out)
      {
        *l_out = l;
      }
      if(w_out)
      {
        *w_out = w;
      }
    }
  }
  return intersection;
}
}

namespace sdl
{
auto Intersection(SDL_FRect const* a, SDL_FRect const* b) -> bool
{
  return Intersects(a->x, a->w, b->x, b->w, nullptr, nullptr) && Intersects(a->y, a->h, b->y, b->h, nullptr, nullptr);
}

auto Intersection(SDL_FRect const* a, SDL_FRect const* b, SDL_FRect* ret) -> bool
{
  return Intersects(a->x, a->w, b->x, b->w, &ret->x, &ret->w) && Intersects(a->y, a->h, b->y, b->h, &ret->y, &ret->h);
}
}