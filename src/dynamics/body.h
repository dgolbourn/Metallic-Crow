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
  auto operator<(Body const& other) const -> bool;
  auto Position() const -> game::Position;
  auto Position(float x, float y) -> void;
  auto Rotation() const -> double;
  auto Rotation(double angle) -> void;
  auto Velocity() const -> game::Position;
  auto Velocity(float x, float y) -> void;
  auto Force(float x, float y) -> void;
  auto Impulse(float x, float y) -> void;
  auto Modulation() const -> display::Modulation;
  auto Emit(float r, float g, float b) -> void;
  auto Intrinsic(float r, float g, float b) -> void;
  typedef memory::WeakPtr<Body, class BodyImpl> WeakPtr;
private:
  std::shared_ptr<class BodyImpl> impl_;
  friend WeakPtr;
  friend class BodyImpl;
  friend class JointImpl;
};
}
#endif
