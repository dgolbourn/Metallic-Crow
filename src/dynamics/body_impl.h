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
  BodyImpl(float x, float y, float u, float v, float w, float h, float m, float c, float d, float k, World& world);
  void Init(float32 x, float32 y, float32 u, float32 v, float32 w, float32 h, float32 m, float32 c, float32 d, float32 k, World& world);
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