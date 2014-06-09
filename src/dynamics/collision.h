#ifndef COLLISION_H_
#define COLLISION_H_
#include <memory>
#include "command.h"
#include "body.h"
#include "queue.h"
namespace game
{
class Collision
{
public:
  Collision(void) = default;
  Collision(event::Queue& queue);
  void Add(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c, bool start);
  bool Check(dynamics::Body const& a, dynamics::Body const& b) const;
  void operator()(dynamics::Body const& a, dynamics::Body const& b, bool start);
  void Clear(void);
private:
  std::shared_ptr<class CollisionImpl> impl_;
};
}
#endif