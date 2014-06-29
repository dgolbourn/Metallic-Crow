#include "world_impl.h"
#include "body_impl.h"
#include "units.h"
#include <vector>
#include "json_iterator.h"
namespace dynamics
{
BodyImpl::BodyImpl(json::JSON const& json, World& world)
{
  json_t* shape;
  json_t* light;
  char const* ctype;
  double dx, dy, du, dv, dm, dc, dd, dk;
  json.Unpack("{s[ff]s[ff]sssosfsfsfsfso}",
    "position", &dx, &dy,
    "velocity", &du, &dv,
    "type", &ctype,
    "shape", &shape,
    "mass", &dm,
    "restitution", &dc,
    "drag", &dd,
    "friction", &dk,
    "light", &light);
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
    std::vector<b2Vec2> vertices(json::JSON(shape).Size());
    auto vertex = vertices.begin();
    for(json::JSON const& value : json::JSON(shape))
    {
      double x, y;
      value.Unpack("[ff]", &x, &y);
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
  position_ = body_->GetPosition();
  velocity_ = body_->GetLinearVelocity();

  if(json::JSON temp = json::JSON(light))
  {
    light_ = Light(temp);
  }
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
  return game::Position(Pixels(position_.x), Pixels(position_.y));
}

void BodyImpl::Position(float x, float y)
{
  body_->SetTransform(b2Vec2(Metres(x), Metres(y)), 0.f);
}

game::Position BodyImpl::Velocity(void) const
{
  return game::Position(Pixels(velocity_.x), Pixels(velocity_.y));
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

void BodyImpl::Begin(void)
{
  if(body_->GetType() != b2_staticBody)
  {
    position_ = body_->GetPosition();
    velocity_ = body_->GetLinearVelocity();
  }
}

void BodyImpl::End(float32 dt)
{
  if(body_->GetType() != b2_staticBody)
  {
    b2Vec2 v = body_->GetLinearVelocity() + velocity_;
    b2Vec2 dx = body_->GetPosition() - position_;
    cubic_[3] = dt * v - 2.f * dx;
    cubic_[2] = -dt * (v + velocity_) + 3.f * dx;
    cubic_[1] = dt * velocity_;
    cubic_[0] = position_;
  }
}

void BodyImpl::Update(float32 ds)
{
  if(body_->GetType() != b2_staticBody)
  {
    position_ = ds * (ds * (ds * cubic_[3] + cubic_[2]) + cubic_[1]) + cubic_[0];
    velocity_ = ds * (ds * 3.f * cubic_[3] + 2.f * cubic_[2]) + cubic_[1];
  }
}

display::Modulation BodyImpl::Modulation(void) const
{
  return display::Modulation(light_.illumination.x, light_.illumination.y, light_.illumination.z, 1.f);
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

display::Modulation Body::Modulation(void) const
{
  return impl_->Modulation();
}
}