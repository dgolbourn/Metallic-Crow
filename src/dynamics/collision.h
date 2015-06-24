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
  auto Begin(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c) -> void;
  auto End(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c) -> void;
  auto Link(dynamics::Body const& a, dynamics::Body const& b) -> void;
  auto Check(dynamics::Body const& a, dynamics::Body const& b) const -> bool;
  auto Begin(dynamics::Body const& a, dynamics::Body const& b) -> void;
  auto End(dynamics::Body const& a, dynamics::Body const& b) -> void;
  auto Unlink(dynamics::Body::WeakPtr const& a) -> void;
  auto Unlink(dynamics::Body::WeakPtr const& a, dynamics::Body::WeakPtr const& b) -> void;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif