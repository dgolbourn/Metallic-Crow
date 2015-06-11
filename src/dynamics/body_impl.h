#ifndef BODY_IMPL_H_
#define BODY_IMPL_H_
#include <memory>
#include "Box2D/Box2D.h"
#include "world.h"
#include "position.h"
#include "body.h"
#include <array>
#include "light.h"
#include "modulation.h"
namespace dynamics
{
typedef std::array<b2Vec2, 4> Cubic;

class BodyImpl final : public std::enable_shared_from_this<BodyImpl>
{
public:
  BodyImpl(lua::Stack& lua, World& world);
  ~BodyImpl();
  game::Position Position() const;
  void Position(float x, float y);
  game::Position Velocity() const;
  void Velocity(float x, float y);
  void Force(float x, float y);
  void Impulse(float x, float y);
  void Begin();
  void End(float32 dt);
  void Update(float32 ds);
  display::Modulation Modulation() const;

  static Body MakeBody(b2Body* body);
  World::WeakPtr world_;
  b2Body* body_;
  b2Vec2 position_;
  b2Vec2 velocity_;
  Cubic cubic_;
  Light light_;
  class Iterator;
};
}
#endif