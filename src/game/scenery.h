#ifndef SCENERY_H_
#define SCENERY_H_
#include "scene.h"
#include "json.h"
#include "window.h"
#include "queue.h"
#include "boost/filesystem.hpp"
namespace game
{
class Scenery
{
public:
  Scenery() = default;
  Scenery(json::JSON const& json, event::Queue& queue, display::Window& window, Scene& scene, boost::filesystem::path const& path);
  void Modulation(float r, float g, float b, float a);
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif