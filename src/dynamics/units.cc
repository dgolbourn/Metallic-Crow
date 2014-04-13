#include "units.h"
namespace dynamics
{
static const float scale = 100.f;

float Metres(float pixels)
{
  return pixels / scale;
}

float Pixels(float metres)
{
  return metres * scale;
}
}