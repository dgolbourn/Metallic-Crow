#ifndef DYNAMICS_COLLISION_H_
#define DYNAMICS_COLLISION_H_
#include <memory>
#include "collision.h"
#include "body.h"
#include "collision_type.h"
namespace game
{
class DynamicsCollision
{
public:
  DynamicsCollision(void) = default;
  DynamicsCollision(Collision const& collision);
  void Add(dynamics::Type group, dynamics::Body const& body);
  void Link(dynamics::Type group_a, dynamics::Type group_b);
private:
  std::shared_ptr<class DynamicsCollisionImpl> impl_;
};
}
#endif