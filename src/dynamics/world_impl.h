#ifndef WORLD_IMPL_H_
#define WORLD_IMPL_H_
#include "Box2D/Box2D.h"
#include "command.h"
#include "signal.h"
#include "collision.h"
#include "queue.h"
#include "timer.h"
#include <memory>
namespace dynamics
{
class WorldImpl final : public b2ContactFilter, public b2ContactListener, public std::enable_shared_from_this<WorldImpl>
{
public:
  WorldImpl(json::JSON const& json, game::Collision& collision, double& t);
  void Init(event::Queue& queue, double t);
  void Step(void);
  void Pause(void);
  void Resume(void);
  void BeginContact(b2Contact* contact);
  void EndContact(b2Contact* contact);
  bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);
  void Begin(event::Command const& command);
  void End(event::Command const& command);
  float32 interval_;
  int32 velocity_iterations_;
  int32 position_iterations_;
  game::Collision collision_;
  event::Signal begin_;
  event::Signal end_;
  b2World world_;
  event::Timer timer_;
};
}
#endif