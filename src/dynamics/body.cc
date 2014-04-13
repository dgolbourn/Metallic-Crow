#include "world_impl.h"
#include "body_impl.h"
#include "units.h"
namespace dynamics
{
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
  x = Metres(x);
  y = Metres(y);
  u = Metres(u);
  v = Metres(v);
  w = Metres(w);
  h = Metres(h);
  b2BodyDef body_def;
  b2FixtureDef fixture_def;
  if(m > 0.f)
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

Body BodyImpl::MakeBody(b2Body* body_ptr)
{
  Body body;
  body.impl_ = ((BodyImpl*)body_ptr->GetUserData())->shared_from_this();
  return body;
}

game::Position BodyImpl::Position(void) const
{
  b2Vec2 position = body_->GetPosition();
  return game::Position(Pixels(position.x), Pixels(position.y));
}

void BodyImpl::Position(float x, float y)
{
  body_->SetTransform(b2Vec2(Metres(x), Metres(y)), 0.f);
}

game::Position BodyImpl::Velocity(void) const
{
  b2Vec2 velocity = body_->GetLinearVelocity();
  return game::Position(Pixels(velocity.x), Pixels(velocity.y));
}

void BodyImpl::Velocity(float x, float y)
{
  body_->SetLinearVelocity(b2Vec2(Metres(x), Metres(y)));
}

void BodyImpl::Force(float x, float y)
{
  body_->ApplyForceToCenter(b2Vec2(Metres(x), Metres(y)), true);
}

void BodyImpl::Impulse(float x, float y)
{
  body_->ApplyLinearImpulse(b2Vec2(Metres(x), Metres(y)), body_->GetWorldCenter(), true);
}

BodyImpl::~BodyImpl(void)
{
  if(world_.Lock())
  {
    body_->GetWorld()->DestroyBody(body_);
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

void Body::Impulse(float x, float y)
{
  impl_->Impulse(x, y);
}
}