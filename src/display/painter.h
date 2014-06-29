#ifndef PAINTER_H_
#define PAINTER_H_
#include "SDL_sysrender.h"
#include "flood_fill.h"
#include <memory>
namespace sdl
{
class Painter
{
public:
  Painter(void) = default;
  Painter(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture, SDL_Rect const* source, SDL_FRect const* destination, double angle, SDL_Color const* modulation);
  bool operator()(algorithm::NodeCoordinates const& coords);
private:
  std::shared_ptr<class PainterImpl> impl_;
};
}
#endif