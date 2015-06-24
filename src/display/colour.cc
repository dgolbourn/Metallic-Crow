#include "colour.h"
#include <cmath>
#include <algorithm>
namespace sdl
{
auto Colour(float colour) -> Uint8
{
  colour *= 255.f;
  colour = std::round(colour);
  colour = std::min(colour, 255.f);
  colour = std::max(colour, 0.f);
  return static_cast<Uint8>(colour);
}
}