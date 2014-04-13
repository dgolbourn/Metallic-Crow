#include "rules_collision.h"
#include "collision_group.h"
namespace game
{
class RulesCollisionImpl final : public CollisionGroup<RulesCollision::Channel, true>
{
  using CollisionGroup::CollisionGroup;
};

void RulesCollision::Add(int group, dynamics::Body const& body, Channel const& channel)
{
  impl_->Add(group, body, channel);
}

void RulesCollision::Link(int group_a, int group_b)
{
  impl_->Link(group_a, group_b);
}

RulesCollision::RulesCollision(Collision const& collision)
{
  impl_ = std::make_shared<RulesCollisionImpl>(collision);
}
}