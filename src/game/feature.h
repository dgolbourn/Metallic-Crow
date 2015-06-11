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
  void Expression(std::string const& expression, int index);
  void Expression(std::string const& expression);
  void Expression(int index);
  bool operator()(display::BoundingBox const& render_box, display::Modulation const& modulation, float parallax, bool facing) const;
  explicit operator bool() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif