#ifndef TERRAIN_H_
#define TERRAIN_H_
#include "scene.h"
#include "json.h"
#include "window.h"
#include "dynamics_collision.h"
#include "world.h"
namespace game
{
class Terrain
{
public:
  Terrain() = default;
  Terrain(json::JSON const& json, display::Window& window, Scene& scene, DynamicsCollision& dcollision, dynamics::World& world);
  void Modulation(float r, float g, float b);
private:
  std::shared_ptr<class TerrainImpl> impl_;
};
}
#endif