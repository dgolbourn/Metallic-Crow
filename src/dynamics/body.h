#ifndef BODY_H_
#define BODY_H_
#include <memory>
#include "weak_ptr.h"
#include "position.h"
#include "command.h"
#include "modulation.h"
#include "lua_stack.h"
namespace dynamics
{
class Body
{
public:
  Body() = default;
  Body(lua::Stack& lua, class World& world);
  explicit operator bool() const;
  bool operator<(Body const& other) const;
  game::Position Position() const;
  void Position(float x, float y);
  game::Position Velocity() const;
  void Velocity(float x, float y);
  void Force(float x, float y);
  void Impulse(float x, float y);
  display::Modulation Modulation() const;
  typedef memory::WeakPtr<Body, class BodyImpl> WeakPtr;
private:
  std::shared_ptr<class BodyImpl> impl_;
  friend WeakPtr;
  friend class BodyImpl;
  friend class JointImpl;
};
}
#endif
