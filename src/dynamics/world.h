#ifndef WORLD_H_
#define WORLD_H_
#include <memory>
#include "json.h"
#include "weak_ptr.h"
#include "command.h"
#include "collision.h"
namespace dynamics
{
class World
{
public:
  World(void) = default;
  World(json::JSON const& json, game::Collision& collision);
  explicit operator bool(void) const;
  void Step(void);
  void Add(event::Command const& command);
  typedef memory::WeakPtr<World, class WorldImpl> WeakPtr;
private:
  std::shared_ptr<class WorldImpl> impl_;
  friend WeakPtr;
  friend class BodyImpl;
};
}
#endif