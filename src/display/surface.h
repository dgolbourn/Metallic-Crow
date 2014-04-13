#ifndef SURFACE_H_
#define SURFACE_H_
#include "font.h"
#include <string>
#include <memory>
#include "SDL_surface.h"
namespace sdl
{
class Surface
{
public:
  Surface(void) = default;
  Surface(std::string const& text, Font const& font, Uint32 length = UINT32_MAX);
  Surface(std::string const& file);
  operator SDL_Surface*(void) const;
private:
  std::shared_ptr<SDL_Surface> impl_;
};
}
#endif