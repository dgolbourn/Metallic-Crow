#ifndef WORLD_IMPL_H_
#define WORLD_IMPL_H_
#include "Box2D/Box2D.h"
#include "command.h"
#include "signal.h"
#include "collision.h"
namespace dynamics
{
class WorldImpl : public b2ContactFilter, public b2ContactListener
{
public:
  WorldImpl(json::JSON const& json, game::Collision& collision);
  void Step(void);
  void BeginContact(b2Contact* contact);
  bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);
  void Add(event::Command const& command);
  b2World world_;
  float32 t_;
  int32 velocity_iterations_;
  int32 position_iterations_;
  game::Collision collision_;
  event::Signal signal_;
};
}
#endif