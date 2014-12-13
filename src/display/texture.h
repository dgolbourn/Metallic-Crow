#ifndef TEXTURE_H_
#define TEXTURE_H_
#include <memory>
#include "bounding_box.h"
#include <string>
#include "font.h"
#include "shape.h"
#include "modulation.h"
#include "boost/filesystem.hpp"
namespace display
{
class Texture
{
public:
  Texture() = default;
  Texture(boost::filesystem::path const& file, class Window& window);
  Texture(std::string const& text, sdl::Font const& font, float width, class Window& window);
  Texture(std::string const& text, sdl::Font const& font, class Window& window);
  Texture(Texture const& texture, BoundingBox const& clip);
  bool operator()(BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle, Modulation const& modulation) const;
  explicit operator bool() const;
  Shape Shape() const;
  class Impl;
private:
  std::shared_ptr<Impl> impl_;
};
}
#endif