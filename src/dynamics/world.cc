#include "world.h"
#include "Box2D/Box2D.h"
#include "command.h"
#include "signal.h"
#include "collision.h"
namespace dynamics
{
class WorldImpl : public b2ContactFilter, public b2ContactListener
{
public:
  WorldImpl(json::JSON const& json, game::Collision& collision);
  Body MakeBody(b2Fixture* fixture);
  void Step(void);
  void BeginContact(b2Contact* contact);
  bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);
  void Add(event::Command const& command);
  b2World world_;
  float32 t_;
  int32 velocity_iterations_;
  int32 position_iterations_;
  game::Collision collision_;
  event::Signal signal_;
};

class BodyImpl : public std::enable_shared_from_this<BodyImpl>
{
public:
  BodyImpl(json::JSON const& json, World& world);
  BodyImpl(float x, float y, float u, float v, float w, float h, float m, float c, float d, float k, World& world);
  void Init(float32 x, float32 y, float32 u, float32 v, float32 w, float32 h, float32 m, float32 c, float32 d, float32 k, World& world);
  ~BodyImpl(void);
  game::Position Position(void) const;
  void Position(float x, float y);
  game::Position Velocity(void) const;
  void Velocity(float x, float y);
  void Force(float x, float y);
  World::WeakPtr world_;
  b2Body* body_;
  b2Fixture* fixture_;
};

Body WorldImpl::MakeBody(b2Fixture* fixture)
{
  Body body;
  body.impl_ = ((BodyImpl*)fixture->GetBody()->GetUserData())->shared_from_this();
  return body;
}

bool WorldImpl::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
{
  return collision_.Check(MakeBody(fixtureA), MakeBody(fixtureB));
}

void WorldImpl::BeginContact(b2Contact* contact)
{
  return collision_.Signal(MakeBody(contact->GetFixtureA()), MakeBody(contact->GetFixtureB()));
}

void WorldImpl::Step(void)
{
  world_.Step(t_, velocity_iterations_, position_iterations_);
  signal_();
}

void WorldImpl::Add(event::Command const& command)
{
  signal_.Add(command);
}

WorldImpl::WorldImpl(json::JSON const& json, game::Collision& collision) : world_(b2Vec2(0, 0)), t_(1.f / 60.f), velocity_iterations_(8), position_iterations_(3), collision_(collision)
{
  double x;
  double y;

  json.Unpack("{s[ff]}",
    "gravity", &x, &y);

  world_.SetGravity(b2Vec2(float32(x),float32(y)));
  world_.SetAutoClearForces(true);
  world_.SetContactListener(this);
  world_.SetContactFilter(this);
}

World::World(json::JSON const& json, game::Collision& collision)
{
  impl_ = std::make_shared<WorldImpl>(json, collision);
}

void World::Step(void)
{
  impl_->Step();
}

void World::Add(event::Command const& command)
{
  impl_->Add(command);
}

World::operator bool(void) const
{
  return bool(impl_);
}

BodyImpl::BodyImpl(json::JSON const& json, World& world)
{
  double x, y, u, v, w, h, m, c, d, k;
  json.Unpack("{s[ff]s[ff]sfsfsfsfsfsf}",
    "position", &x, &y,
    "velocity", &u, &v,
    "width", &w,
    "height", &h,
    "mass", &m,
    "restitution", &c,
    "drag", &d,
    "friction", &k);
  Init((float32)x, (float32)y, (float32)u, (float32)v, (float32)w, (float32)h, (float32)m, (float32)c, (float32)d, (float32)k, world);
}

BodyImpl::BodyImpl(float x, float y, float u, float v, float w, float h, float m, float c, float d, float k, World& world)
{
  Init((float32)x, (float32)y, (float32)u, (float32)v, (float32)w, (float32)h, (float32)m, (float32)c, (float32)d, (float32)k, world);
}

void BodyImpl::Init(float32 x, float32 y, float32 u, float32 v, float32 w, float32 h, float32 m, float32 c, float32 d, float32 k, World& world)
{
  b2BodyDef body_def;
  b2FixtureDef fixture_def;
  if(std::isfinite(m) && (m > 0.f))
  {
    body_def.type = b2_dynamicBody;
    fixture_def.density = m / (w * h);
  }
  else
  {
    if(m == 0.f)
    {
      fixture_def.isSensor = true;
    }
    if((u == 0.f) && (v == 0.f))
    {
      body_def.type = b2_staticBody;
    }
    else
    {
      body_def.type = b2_kinematicBody;
    }
    fixture_def.density = 0.f;
  }

  body_def.position.Set(x, y);
  body_def.linearVelocity.Set(u, v);
  body_def.angularVelocity = 0.f;
  body_def.linearDamping = d;
  body_def.angularDamping = 0.f;
  body_def.fixedRotation = true;
  b2PolygonShape box;
  box.SetAsBox(.5f * w, .5f * h, b2Vec2(0.f, 0.f), 0.f);
  fixture_def.shape = &box;
  fixture_def.friction = k;
  fixture_def.restitution = c;
  body_ = world.impl_->world_.CreateBody(&body_def);
  body_->SetUserData(this);
  fixture_ = body_->CreateFixture(&fixture_def);
  world_ = world;
}

game::Position BodyImpl::Position(void) const
{
  b2Vec2 position = body_->GetPosition();
  return game::Position(position.x, position.y);
}

void BodyImpl::Position(float x, float y)
{
  body_->SetTransform(b2Vec2(x, y), 0.f);
}

game::Position BodyImpl::Velocity(void) const
{
  b2Vec2 velocity = body_->GetLinearVelocity();
  return game::Position(velocity.x, velocity.y);
}

void BodyImpl::Velocity(float x, float y)
{
  body_->SetLinearVelocity(b2Vec2(x, y));
}

void BodyImpl::Force(float x, float y)
{
  body_->ApplyForceToCenter(b2Vec2(x, y), true);
}

BodyImpl::~BodyImpl(void)
{
  if(auto world = world_.Lock())
  {
    world.impl_->world_.DestroyBody(body_);
  }
}

Body::Body(json::JSON const& json, World& world)
{
  impl_ = std::make_shared<BodyImpl>(json, world);
}

Body::Body(float x, float y, float u, float v, float w, float h, float m, float c, float d, float k, World& world)
{
  impl_ = std::make_shared<BodyImpl>(x, y, u, v, w, h, m, c, d, k, world);
}

bool Body::operator<(Body const& other) const
{
  return impl_.owner_before(other.impl_);
}

Body::operator bool(void) const
{
  return bool(impl_) && bool(impl_->world_.Lock());
}

game::Position Body::Position(void) const
{
  return impl_->Position();
}

void Body::Position(float x, float y)
{
  impl_->Position(x, y);
}

game::Position Body::Velocity(void) const
{
  return impl_->Velocity();
}

void Body::Velocity(float x, float y)
{
  impl_->Velocity(x, y);
}

void Body::Force(float x, float y)
{
  impl_->Force(x, y);
}
}