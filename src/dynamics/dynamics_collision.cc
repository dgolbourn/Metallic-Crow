#include "dynamics_collision.h"
#include "collision_group.h"
#include "channel.h"
#include "make_channel.h"
namespace game
{
class DynamicsCollisionImpl final : public CollisionGroup<dynamics::Channel, true>
{
  using CollisionGroup::CollisionGroup;
};

void DynamicsCollision::Add(int group, dynamics::Body const& body)
{
  impl_->Add(group, body, dynamics::MakeChannel());
}

void DynamicsCollision::Link(int group_a, int group_b)
{
  impl_->Link(group_a, group_b);
}

DynamicsCollision::DynamicsCollision(Collision const& collision)
{
  impl_ = std::make_shared<DynamicsCollisionImpl>(collision);
}
}