#ifndef WORLD_H_
#define WORLD_H_
#include <memory>
#include "json.h"
#include "weak_ptr.h"
#include "command.h"
#include "collision.h"
#include "queue.h"
namespace dynamics
{
class World
{
public:
  World(void) = default;
  World(json::JSON const& json, collision::Collision& collision, event::Queue& queue);
  explicit operator bool(void) const;
  void Begin(event::Command const& command);
  void End(event::Command const& command);
  void Pause(void);
  void Resume(void);
  void Ambient(float r, float g, float b);
  bool operator==(World const& other) const;
  typedef memory::WeakPtr<World, class WorldImpl> WeakPtr;
private:
  std::shared_ptr<class WorldImpl> impl_;
  friend WeakPtr;
  friend class BodyImpl;
  friend class JointImpl;
};
}
#endif