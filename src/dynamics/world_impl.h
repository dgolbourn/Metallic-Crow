#ifndef WORLD_IMPL_H_
#define WORLD_IMPL_H_
#include "Box2D/Box2D.h"
#include "command.h"
#include "signal.h"
#include "collision_group.h"
#include "queue.h"
#include "timer.h"
#include <memory>
#include <chrono>
#include <map>
#include "body_impl_pair.h"
namespace dynamics
{
typedef std::chrono::high_resolution_clock Clock;
typedef std::map<BodyImplPair, int> BodyImplCount;

class WorldImpl final : public b2DestructionListener, public b2ContactFilter, public b2ContactListener, public std::enable_shared_from_this<WorldImpl>
{
public:
  WorldImpl(lua::Stack& lua, collision::Group& collision);
  auto Init(event::Queue& queue) -> void;
  auto Update() -> void;
  auto Pause() -> void;
  auto Resume() -> void;
  auto BeginContact(b2Contact* contact) -> void override;
  auto EndContact(b2Contact* contact) -> void override;
  auto ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB) -> bool override;
  auto Begin(event::Command const& command) -> void;
  auto End(event::Command const& command) -> void;
  auto Light() -> void;
  auto Ambient(float r, float g, float b) -> void;
  auto SayGoodbye(b2Joint* joint) -> void override;
  auto SayGoodbye(b2Fixture* fixture) -> void override;
  auto Metres(double pixels) const -> float32;
  auto Pixels(float32 metres) const -> float;
  float32 f_;
  float32 dt_;
  collision::Group collision_;
  event::Signal begin_;
  event::Signal end_;
  b2World world_;
  Clock::duration interval_;
  Clock::time_point tick_;
  Clock::duration remaining_;
  bool paused_;
  b2Vec3 ambient_;
  BodyImplCount contact_;
  float32 scale_;
};
}
#endif