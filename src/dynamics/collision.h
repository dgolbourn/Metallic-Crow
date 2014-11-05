#ifndef COLLISION_H_
#define COLLISION_H_
#include <memory>
#include "command.h"
#include "body.h"
#include "queue.h"
namespace collision
{
class Collision
{
public:
  Collision() = default;
  Collision(event::Queue& queue);
  void Begin(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c);
  void End(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c);
  void Link(dynamics::Body const& a, dynamics::Body const& b);
  bool Check(dynamics::Body const& a, dynamics::Body const& b) const;
  void Begin(dynamics::Body const& a, dynamics::Body const& b);
  void End(dynamics::Body const& a, dynamics::Body const& b);
  void Unlink(dynamics::Body::WeakPtr const& a);
  void Unlink(dynamics::Body::WeakPtr const& a, dynamics::Body::WeakPtr const& b);
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif