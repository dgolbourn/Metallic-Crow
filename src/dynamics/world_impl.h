#ifndef WORLD_IMPL_H_
#define WORLD_IMPL_H_
#include "Box2D/Box2D.h"
#include "command.h"
#include "signal.h"
#include "collision.h"
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
  WorldImpl(json::JSON const& json, collision::Collision& collision);
  void Init(event::Queue& queue);
  void Update(void);
  void Pause(void);
  void Resume(void);
  void BeginContact(b2Contact* contact);
  void EndContact(b2Contact* contact);
  bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);
  void Begin(event::Command const& command);
  void End(event::Command const& command);
  void Light(void);
  void Ambient(float r, float g, float b);
  void SayGoodbye(b2Joint* joint);
  void SayGoodbye(b2Fixture* fixture);
  float32 Metres(double pixels) const;
  float Pixels(float32 metres) const;
  float32 f_;
  float32 dt_;
  collision::Collision collision_;
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