#include "dynamics_collision.h"
#include "collision_group.h"
#include "channel.h"
#include "make_channel.h"
namespace game
{
class DynamicsCollisionImpl final : public CollisionGroup<dynamics::Channel, true>
{
public:
  using CollisionGroup::CollisionGroup;
  void Init(void);
};

void DynamicsCollisionImpl::Init(void)
{
  Link(dynamics::Type::Hero, dynamics::Type::Body);
  Link(dynamics::Type::Body, dynamics::Type::Body);
}

void DynamicsCollision::Add(dynamics::Type group, dynamics::Body const& body)
{
  impl_->Add(group, body, dynamics::MakeChannel());
}

void DynamicsCollision::Link(dynamics::Type group_a, dynamics::Type group_b)
{
  impl_->Link(group_a, group_b);
}

DynamicsCollision::DynamicsCollision(Collision const& collision)
{
  impl_ = std::make_shared<DynamicsCollisionImpl>(collision);
  impl_->Init();
}
}