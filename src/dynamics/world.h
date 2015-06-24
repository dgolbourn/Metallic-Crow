#ifndef WORLD_H_
#define WORLD_H_
#include <memory>
#include "lua_stack.h"
#include "weak_ptr.h"
#include "command.h"
#include "collision.h"
#include "queue.h"
namespace dynamics
{
class World
{
public:
  World() = default;
  World(lua::Stack& lua, collision::Collision& collision, event::Queue& queue);
  explicit operator bool() const;
  auto Begin(event::Command const& command) -> void;
  auto End(event::Command const& command) -> void;
  auto Pause() -> void;
  auto Resume() -> void;
  auto Ambient(float r, float g, float b) -> void;
  auto operator==(World const& other) const -> bool;
  typedef memory::WeakPtr<World, class WorldImpl> WeakPtr;
private:
  std::shared_ptr<class WorldImpl> impl_;
  friend WeakPtr;
  friend class BodyImpl;
  friend class JointImpl;
};
}
#endif