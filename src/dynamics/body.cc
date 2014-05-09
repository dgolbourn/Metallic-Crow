#include "world_impl.h"
#include "body_impl.h"
#include "units.h"
#include <vector>
#include "jansson.h"
namespace dynamics
{
BodyImpl::BodyImpl(json::JSON const& json, World& world)
{
  json_t* shape;
  char const* ctype;
  double dx, dy, du, dv, dm, dc, dd, dk;
  json.Unpack("{s[ff]s[ff]sssosfsfsfsf}",
    "position", &dx, &dy,
    "velocity", &du, &dv,
    "type", &ctype,
    "shape", &shape,
    "mass", &dm,
    "restitution", &dc,
    "drag", &dd,
    "friction", &dk);
  float x = Metres(float(dx));
  float y = Metres(float(dy));
  float u = Metres(float(du));
  float v = Metres(float(dv));
  float mass = float(dm);
  float c = float(dc);
  float d = float(dd);
  float k = float(dk);
  std::string type(ctype);

  b2FixtureDef fixture_def;
  b2PolygonShape box;
  b2CircleShape circle;
  b2ChainShape chain;
  float area;
 
  if(type == "box")
  {
    double dwidth, dheight;
    json::JSON(shape).Unpack("[ff]", &dwidth, &dheight);
    float width = Metres(float(dwidth));
    float height = Metres(float(dheight));
    box.SetAsBox(.5f * width, .5f * height);
    fixture_def.shape = &box;
    area = width * height;
  }
  else if(type == "circle")
  {
    double dradius;
    json::JSON(shape).Unpack("f", &dradius);
    float radius = Metres(float(dradius));
    circle.m_p.SetZero();
    circle.m_radius = radius;
    fixture_def.shape = &circle;
    area = radius * radius * b2_pi;
  }
  else if(type == "chain")
  {
    std::vector<b2Vec2> vertices(json_array_size(shape));
    auto vertex = vertices.begin();
    size_t index;
    json_t* value;
    json_array_foreach(shape, index, value)
    {
      double x, y;
      json_unpack(value, "[ff]", &x, &y);
      vertex->Set(Metres(float(x)),Metres(float(y)));
      ++vertex;
    }

    if(vertices.front() == vertices.back())
    {
      vertices.pop_back();
      chain.CreateChain(vertices.data(), vertices.size());
      chain.CreateLoop(vertices.data(), vertices.size());
    }
    else
    {
      chain.CreateChain(vertices.data(), vertices.size());
    }
    fixture_def.shape = &chain;
    area = 0.f;
  }
  else
  {
  }

  b2BodyDef body_def;
  if(mass > 0.f && area > 0.f)
  {
    body_def.type = b2_dynamicBody;
    fixture_def.density = mass / area;
  }
  else
  {
    if(mass == 0.f)
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
  if(body_ptr)
  {
    BodyImpl* impl = (BodyImpl*)body_ptr->GetUserData();
    if(impl)
    {
      body.impl_ = impl->shared_from_this();
    }
  }
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
  if(auto world = world_.Lock())
  {
    body_->SetUserData(nullptr);
    body_->GetWorld()->DestroyBody(body_);
  }
}

Body::Body(json::JSON const& json, World& world)
{
  impl_ = std::make_shared<BodyImpl>(json, world);
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