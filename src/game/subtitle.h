#ifndef SUBTITLE_H_
#define SUBTITLE_H_
#include "lua_stack.h"
#include "window.h"
#include <memory>
#include "boost/filesystem.hpp"
namespace game
{
class Subtitle
{
public:
  Subtitle() = default;
  Subtitle(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path);
  auto operator()(std::string const& text) -> void;
  auto Modulation(float r, float g, float b, float a) -> void;
  auto Render() const -> void;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif