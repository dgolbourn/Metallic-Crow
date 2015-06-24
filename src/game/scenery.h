#ifndef SCENERY_H_
#define SCENERY_H_
#include "scene.h"
#include "lua_stack.h"
#include "window.h"
#include "queue.h"
#include "boost/filesystem.hpp"
namespace game
{
class Scenery
{
public:
  Scenery() = default;
  Scenery(lua::Stack& lua, event::Queue& queue, display::Window& window, Scene& scene, boost::filesystem::path const& path);
  auto Modulation(float r, float g, float b, float a) -> void;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif