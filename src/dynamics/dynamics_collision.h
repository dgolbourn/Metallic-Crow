#ifndef DYNAMICS_COLLISION_H_
#define DYNAMICS_COLLISION_H_
#include <memory>
#include "collision.h"
#include "body.h"
namespace game
{
class DynamicsCollision
{
public:
  DynamicsCollision(void) = default;
  DynamicsCollision(Collision const& collision);
  void Add(int group, dynamics::Body const& body);
  void Link(int group_a, int group_b);
private:
  std::shared_ptr<class DynamicsCollisionImpl> impl_;
};
}
#endif