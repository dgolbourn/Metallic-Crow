#ifndef SET_H_
#define SET_H_
#include "scene.h"
#include "json.h"
#include "window.h"
#include "collision_group.h"
#include "world.h"
namespace game
{
class Set
{
public:
  Set() = default;
  Set(json::JSON const& json, display::Window& window, Scene& scene, collision::Group& collision, dynamics::World& world);
  void Modulation(float r, float g, float b);
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif