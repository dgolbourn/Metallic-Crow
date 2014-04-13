#ifndef BOX_H_
#define BOX_H_
#include "scene.h"
#include "json.h"
#include "window.h"
#include "dynamics_collision.h"
#include "world.h"
#include "queue.h"
namespace game
{
class Box
{
public:
  Box() = default;
  Box(json::JSON const& json, display::Window& window, Scene& scene, event::Queue& queue, DynamicsCollision& dcollision, dynamics::World& world);
  void Position(game::Position const& position);
  game::Position Position(void) const;
  void Pause(void);
  void Resume(void);
private:
  std::shared_ptr<class BoxImpl> impl_;
};
}
#endif