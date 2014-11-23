#include "game_joint.h"
#include "joint.h"
#include "actor_impl.h"
namespace game
{
class JointImpl
{
public:
  JointImpl(json::JSON const& json, Actor const& actor_a, Actor const& actor_b, dynamics::World& world);
  dynamics::Joint joint_;
};

JointImpl::JointImpl(json::JSON const& json, Actor const& actor_a, Actor const& actor_b, dynamics::World& world) : joint_(json, actor_a.impl_->body_, actor_b.impl_->body_, world)
{
}

Joint::Joint(json::JSON const& json, Actor const& actor_a, Actor const& actor_b, dynamics::World& world) : impl_(std::make_shared<JointImpl>(json, actor_a, actor_b, world))
{
}
}