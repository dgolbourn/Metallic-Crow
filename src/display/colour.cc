#include "colour.h"
#include <cmath>
#include <algorithm>
namespace sdl
{
Uint8 Colour(float colour)
{
  colour *= 255.f;
  colour = std::round(colour);
  colour = std::min(colour, 255.f);
  colour = std::max(colour, 0.f);
  return Uint8(colour);
}
}