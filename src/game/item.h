#ifndef ITEM_H_
#define ITEM_H_
#include "scene.h"
#include "json.h"
#include "window.h"
#include "dynamics_collision.h"
#include "command_collision.h"
#include "world.h"
#include "queue.h"
namespace game
{
class Item
{
public:
  Item() = default;
  Item(json::JSON const& json, display::Window& window, Scene& scene, event::Queue& queue, DynamicsCollision& dcollision, CommandCollision& ccollision, dynamics::World& world);
  void Position(game::Position const& position);
  game::Position Position(void) const;
  void Pause(void);
  void Resume(void);
private:
  std::shared_ptr<class ItemImpl> impl_;
};
}
#endif