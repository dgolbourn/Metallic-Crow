#ifndef FEATURE_H_
#define FEATURE_H_
#include <memory>
#include "window.h"
#include "lua_stack.h"
#include "boost/filesystem.hpp"
namespace game
{
class Feature
{
public:
  Feature() = default;
  Feature(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path);
  auto Expression(std::string const& expression, int index) -> void;
  auto Expression(std::string const& expression) -> void;
  auto Expression(int index) -> void;
  auto operator()(display::BoundingBox const& render_box, display::Modulation const& modulation, float parallax, bool facing) const -> bool;
  explicit operator bool() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif