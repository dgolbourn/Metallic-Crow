#include "dynamics_collision.h"
#include "collision_group.h"
namespace game
{
typedef event::Send<bool> Send;
typedef event::Receive<bool> Receive;
typedef event::Channel<bool> Channel;

class DynamicsCollisionImpl final : public CollisionGroup<Channel>
{
  using CollisionGroup::CollisionGroup;
};

void DynamicsCollision::Add(int group, dynamics::Body const& body)
{
  static const Send send = [=](void)
  {
    return std::pair<bool, bool>(true, true);
  };
  static const Receive receive = [=](bool)
  {
    return true;
  };
  static const Channel channel(send, receive);
  impl_->Add(group, body, channel);
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