#include "game_joint.h"
#include "joint.h"
#include "actor_impl.h"
namespace game
{
class JointImpl
{
public:
  JointImpl(lua::Stack& lua, Actor const& actor_a, Actor const& actor_b, dynamics::World& world);
  dynamics::Joint joint_;
};

JointImpl::JointImpl(lua::Stack& lua, Actor const& actor_a, Actor const& actor_b, dynamics::World& world) : joint_(lua, actor_a.impl_->dynamics_body_, actor_b.impl_->dynamics_body_, world)
{
}

Joint::Joint(lua::Stack& lua, Actor const& actor_a, Actor const& actor_b, dynamics::World& world) : impl_(std::make_shared<JointImpl>(lua, actor_a, actor_b, world))
{
}
}