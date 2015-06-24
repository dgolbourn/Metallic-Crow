#ifndef SCREEN_H_
#define SCREEN_H_
#include "scene.h"
#include "lua_stack.h"
#include "queue.h"
#include "window.h"
#include "boost/filesystem.hpp"
namespace game
{
class Screen
{
public:
  Screen() = default;
  Screen(lua::Stack& lua, display::Window& window, Scene& scene, event::Queue& queue, boost::filesystem::path const& path);
  auto Pause() -> void;
  auto Resume() -> void;
  auto Modulation(float r, float g, float b, float a) -> void;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif