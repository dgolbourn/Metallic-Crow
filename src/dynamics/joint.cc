#include "joint.h"
#include "Box2D/Box2D.h"
#include "world_impl.h"
#include <string>
#include "exception.h"
#include "body_impl.h"
namespace dynamics
{
class JointImpl
{
public:
  JointImpl(json::JSON const& json, Body& body_a, Body& body_b, World& world);
  ~JointImpl(void);
  bool Valid(void) const;
  b2Joint* joint_;
  World::WeakPtr world_;
};

typedef std::unique_ptr<b2JointDef> JointDefinition;

static JointDefinition Spring(json::JSON const& json, b2Body* body_a, b2Body* body_b, WorldImpl const& world)
{
  double length;
  json_t* damping_ptr;
  json.Unpack("{sfso}",
    "length", &length,
    "damping", &damping_ptr);

  JointDefinition def(new b2DistanceJointDef);
  b2DistanceJointDef& joint = *(b2DistanceJointDef*)def.get();
  joint.length = float32(length);
  if(auto damping = json::JSON(damping_ptr))
  {
    double temp;
    damping.Unpack("f", &temp);
    joint.dampingRatio = float32(temp);
    joint.frequencyHz = 0.25f * world.f_;
  }
  joint.bodyA = body_a;
  joint.bodyB = body_b;

  return def;
}

static JointDefinition Rope(json::JSON const& json, b2Body* body_a, b2Body* body_b)
{
  double length;
  json.Unpack("{sf}",
    "length", &length);

  JointDefinition def(new b2RopeJointDef);
  b2RopeJointDef& joint = *(b2RopeJointDef*)def.get();
  joint.maxLength = float32(length);
  joint.bodyA = body_a;
  joint.bodyB = body_b;

  return def;
}

static JointDefinition Weld(json::JSON const& json, b2Body* body_a, b2Body* body_b, WorldImpl const& world)
{
  json_t* damping_ptr;
  json.Unpack("{so}",
    "damping", &damping_ptr);

  b2Body* a = body_a;
  b2Body* b = body_b;
  b2Vec2 anchor = a->GetPosition();
  anchor += b->GetPosition();
  anchor *= .5f;

  JointDefinition def(new b2WeldJointDef);
  b2WeldJointDef& joint = *(b2WeldJointDef*)def.get();
  joint.Initialize(a, b, anchor);
  if(auto damping = json::JSON(damping_ptr))
  {
    double temp;
    damping.Unpack("f", &temp);
    joint.dampingRatio = float32(temp);
    joint.frequencyHz = 0.25f * world.f_;
  }
  
  return def;
}

JointImpl::JointImpl(json::JSON const& json, Body& body_a, Body& body_b, World& world) : joint_(nullptr), world_(world)
{
  char const* type_ptr;
  json_t* json_ptr;
  int collide;
  json.Unpack("{sssosb}", 
    "type", &type_ptr,
    "joint", &json_ptr,
    "collision", &collide);

  std::string type(type_ptr);
  JointDefinition def;
  if(type == "spring")
  {
    def = Spring(json::JSON(json_ptr), body_a.impl_->body_, body_b.impl_->body_, *world.impl_.get());
  }
  else if(type == "rope")
  {
    def = Rope(json::JSON(json_ptr), body_a.impl_->body_, body_b.impl_->body_);
  }
  else if(type == "weld")
  {
    def = Weld(json::JSON(json_ptr), body_a.impl_->body_, body_b.impl_->body_, *world.impl_.get());
  }
  else
  {
    BOOST_THROW_EXCEPTION(exception::Exception());
  }

  def->collideConnected = (collide != 0);
  def->userData = this;
  joint_ = world.impl_->world_.CreateJoint(def.get());
}

JointImpl::~JointImpl(void)
{
  if(joint_)
  {
    if(auto world = world_.Lock())
    {
      world.impl_->world_.DestroyJoint(joint_);
    }
  }
}

bool JointImpl::Valid(void) const
{
  return (joint_ != nullptr) && bool(world_.Lock());
}

Joint::operator bool(void) const
{
  return bool(impl_) && impl_->Valid();
}

Joint::Joint(json::JSON const& json, Body& body_a, Body& body_b, World& world)
{
  impl_ = std::make_shared<JointImpl>(json, body_a, body_b, world);
}
}