#include "colour.h"
#include <cmath>
namespace display
{
auto Colour(float colour) -> Uint8
{
  colour *= 255.f;
  colour = std::round(colour);
  colour = std::fmin(colour, 255.f);
  colour = std::fmax(colour, 0.f);
  return static_cast<Uint8>(colour);
}
}