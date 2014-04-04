#ifndef COLLISION_H_
#define COLLISION_H_
#include <memory>
#include "command.h"
#include "world.h"
namespace game
{
class Collision
{
public:
  Collision(void);
  void Add(dynamics::Body const& a, dynamics::Body const& b, event::Command const& c);
  bool Check(dynamics::Body const& a, dynamics::Body const& b) const;
  void Signal(dynamics::Body const& a, dynamics::Body const& b);
  void Clear(void);
private:
  std::shared_ptr<class CollisionImpl> impl_;
};
}
#endif