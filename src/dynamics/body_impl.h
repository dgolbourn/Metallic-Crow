#ifndef BODY_IMPL_H_
#define BODY_IMPL_H_
#include <memory>
#include "Box2D/Box2D.h"
#include "world.h"
#include "position.h"
#include "body.h"
namespace dynamics
{
class BodyImpl final : public std::enable_shared_from_this<BodyImpl>
{
public:
  BodyImpl(json::JSON const& json, World& world);
  ~BodyImpl(void);
  game::Position Position(void) const;
  void Position(float x, float y);
  game::Position Velocity(void) const;
  void Velocity(float x, float y);
  void Force(float x, float y);
  void Impulse(float x, float y);
  static Body MakeBody(b2Body* body);
  World::WeakPtr world_;
  b2Body* body_;
  b2Fixture* fixture_;
};
}
#endif