#ifndef BODY_H_
#define BODY_H_
#include <memory>
#include "json.h"
#include "weak_ptr.h"
#include "position.h"
#include "command.h"
namespace dynamics
{
class Body
{
public:
  Body(void) = default;
  Body(json::JSON const& json, class World& world);
  explicit operator bool(void) const;
  bool operator<(Body const& other) const;
  game::Position Position(void) const;
  void Position(float x, float y);
  game::Position Velocity(void) const;
  void Velocity(float x, float y);
  void Force(float x, float y);
  void Impulse(float x, float y);
  typedef memory::WeakPtr<Body, class BodyImpl> WeakPtr;
private:
  std::shared_ptr<class BodyImpl> impl_;
  friend WeakPtr;
  friend class BodyImpl;
};
}
#endif
