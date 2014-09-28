#ifndef TEXTURE_H_
#define TEXTURE_H_
#include <memory>
#include "bounding_box.h"
#include <string>
#include "font.h"
#include "shape.h"
#include "modulation.h"
namespace display
{
class Texture
{
public:
  Texture(void) = default;
  Texture(std::string const& file, class Window& window);
  Texture(std::string const& text, sdl::Font const& font, int width, class Window& window);
  Texture(std::string const& text, sdl::Font const& font, class Window& window);
  Texture(Texture const& texture, BoundingBox const& clip);
  bool operator()(BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle, Modulation const& modulation) const;
  explicit operator bool(void) const;
  Shape Shape(void) const;
  class Impl;
private:
  std::shared_ptr<Impl> impl_;
};
}
#endif