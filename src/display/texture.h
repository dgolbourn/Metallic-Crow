#ifndef TEXTURE_H_
#define TEXTURE_H_
#include <memory>
#include "bounding_box.h"
#include <string>
#include "font.h"
#include "shape.h"
#include "modulation.h"
#include "boost/filesystem.hpp"
#include "timeslice.h"
namespace display
{
class Texture
{
public:
  Texture() = default;
  Texture(boost::filesystem::path const& file, class Window& window, event::Timeslice& loader, BoundingBox const& clip);
  Texture(std::string const& text, sdl::Font const& font, float width, class Window& window, BoundingBox const& clip);
  Texture(std::string const& text, sdl::Font const& font, class Window& window, BoundingBox const& clip);
  auto operator()(BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle, Modulation const& modulation) -> bool;
  explicit operator bool() const;
  auto Shape() const -> Shape;
  class Impl;
private:
  std::shared_ptr<Impl> impl_;
};
}
#endif