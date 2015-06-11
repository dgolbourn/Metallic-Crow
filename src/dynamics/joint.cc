#include "joint.h"
#include "Box2D/Box2D.h"
#include "world_impl.h"
#include <string>
#include "exception.h"
#include "body_impl.h"
#include "log.h"
namespace dynamics
{
class JointImpl
{
public:
  JointImpl(lua::Stack& lua, Body& body_a, Body& body_b, World& world);
  ~JointImpl();
  bool Valid() const;
  b2Joint* joint_;
  World::WeakPtr world_;
};

typedef std::unique_ptr<b2JointDef> JointDefinition;

static JointDefinition Spring(lua::Stack& lua, b2Body* body_a, b2Body* body_b, WorldImpl const& world)
{
  JointDefinition def(new b2DistanceJointDef);
  b2DistanceJointDef& joint = *(b2DistanceJointDef*)def.get();

  {
    lua::Guard guard = lua.Field("length");
    lua.Pop(joint.length);
  }

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

static JointDefinition Rope(lua::Stack& lua, b2Body* body_a, b2Body* body_b)
{
  JointDefinition def(new b2RopeJointDef);
  b2RopeJointDef& joint = *(b2RopeJointDef*)def.get();

  {
    lua::Guard guard = lua.Field("length");
    lua.Pop(joint.maxLength);
  }

  joint.bodyA = body_a;
  joint.bodyB = body_b;

  return def;
}

static JointDefinition Weld(lua::Stack& lua, b2Body* body_a, b2Body* body_b, WorldImpl const& world)
{
  b2Body* a = body_a;
  b2Body* b = body_b;
  b2Vec2 anchor = a->GetPosition();
  anchor += b->GetPosition();
  anchor *= .5f;

  JointDefinition def(new b2WeldJointDef);
  b2WeldJointDef& joint = *(b2WeldJointDef*)def.get();
  joint.Initialize(a, b, anchor);

  lua::Guard guard = lua.Field("damping");
  if(lua.Check())
  {
    lua.Pop(joint.dampingRatio);
    joint.frequencyHz = 0.25f * world.f_;
  }
  
  return def;
}

JointImpl::JointImpl(lua::Stack& lua, Body& body_a, Body& body_b, World& world) : joint_(nullptr), world_(world)
{
  std::string type;
  {
    lua::Guard guard = lua.Field("type");
    lua.Pop(type);
  }

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

bool JointImpl::Valid() const
{
  return (joint_ != nullptr) && bool(world_.Lock());
}

Joint::operator bool() const
{
  return bool(impl_) && impl_->Valid();
}

Joint::Joint(lua::Stack& lua, Body& body_a, Body& body_b, World& world)
{
  impl_ = std::make_shared<JointImpl>(lua, body_a, body_b, world);
}
}