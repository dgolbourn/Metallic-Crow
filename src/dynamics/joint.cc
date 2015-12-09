#include "joint.h"
#include "Box2D/Box2D.h"
#include "world_impl.h"
#include <string>
#include "exception.h"
#include "body_impl.h"
#include "log.h"
namespace 
{
typedef std::unique_ptr<b2JointDef> JointDefinition;

auto Spring(lua::Stack& lua, b2Body* body_a, b2Body* body_b, dynamics::WorldImpl const& world) -> JointDefinition 
{
  JointDefinition def(new b2DistanceJointDef);
  b2DistanceJointDef& joint = *static_cast<b2DistanceJointDef*>(def.get());

  joint.length = lua.Field<float>("length");

  lua::Guard guard = lua.Field("damping");
  if(lua.Check())
  {
    lua.Pop(joint.dampingRatio);
    joint.frequencyHz = 0.25f * world.f_;
  }

  joint.bodyA = body_a;
  joint.bodyB = body_b;

  return def;
}

auto Rope(lua::Stack& lua, b2Body* body_a, b2Body* body_b) -> JointDefinition
{
  JointDefinition def(new b2RopeJointDef);
  b2RopeJointDef& joint = *static_cast<b2RopeJointDef*>(def.get());

  joint.maxLength = lua.Field<float>("length");
    
  joint.bodyA = body_a;
  joint.bodyB = body_b;

  return def;
}

auto Weld(lua::Stack& lua, b2Body* body_a, b2Body* body_b, dynamics::WorldImpl const& world) -> JointDefinition
{
  b2Body* a = body_a;
  b2Body* b = body_b;
  b2Vec2 anchor = a->GetPosition();
  anchor += b->GetPosition();
  anchor *= .5f;

  JointDefinition def(new b2WeldJointDef);
  b2WeldJointDef& joint = *static_cast<b2WeldJointDef*>(def.get());
  joint.Initialize(a, b, anchor);

  lua::Guard guard = lua.Field("damping");
  if(lua.Check())
  {
    lua.Pop(joint.dampingRatio);
    joint.frequencyHz = 0.25f * world.f_;
  }
  
  return def;
}
}

namespace dynamics
{
class JointImpl
{
public:
  JointImpl(lua::Stack& lua, Body& body_a, Body& body_b, World& world);
  ~JointImpl();
  auto Valid() const -> bool;
  b2Joint* joint_;
  World::WeakPtr world_;
};

JointImpl::JointImpl(lua::Stack& lua, Body& body_a, Body& body_b, World& world) : joint_(nullptr), world_(world)
{
  std::string type = lua.Field<std::string>("type");

  JointDefinition def;
  if(type == "spring")
  {
    lua::Guard guard = lua.Field("joint");
    def = Spring(lua, body_a.impl_->body_, body_b.impl_->body_, *world.impl_.get());
  }
  else if(type == "rope")
  {
    lua::Guard  guard = lua.Field("joint");
    def = Rope(lua, body_a.impl_->body_, body_b.impl_->body_);
  }
  else if(type == "weld")
  {
    lua::Guard guard = lua.Field("joint");
    def = Weld(lua, body_a.impl_->body_, body_b.impl_->body_, *world.impl_.get());
  }
  else
  {
    BOOST_THROW_EXCEPTION(exception::Exception());
  }

  {
    lua::Guard guard = lua.Field("collision");
    lua.Pop(def->collideConnected);
  }

  def->userData = this;
  joint_ = world.impl_->world_.CreateJoint(def.get());
}

JointImpl::~JointImpl()
{
  try
  {
    if(joint_)
    {
      if(auto world = world_.Lock())
      {
        world.impl_->world_.DestroyJoint(joint_);
      }
    }
  }
  catch(...)
  {
    exception::Log("Swallowed exception");
  }
}

auto JointImpl::Valid() const -> bool
{
  return (joint_ != nullptr) && static_cast<bool>(world_.Lock());
}

Joint::operator bool() const
{
  return static_cast<bool>(impl_) && impl_->Valid();
}

Joint::Joint(lua::Stack& lua, Body& body_a, Body& body_b, World& world) : impl_(std::make_shared<JointImpl>(lua, body_a, body_b, world))
{
}
}