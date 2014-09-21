#include "world_impl.h"
#include "body_impl.h"
#include <vector>
#include "json_iterator.h"
#include "exception.h"
namespace dynamics
{
typedef std::unique_ptr<b2Shape> Shape;
typedef std::pair<Shape, float32> ShapePair;

static ShapePair Box(WorldImpl const& world, json::JSON& json)
{
  double x, y, w, h;
  json.Unpack("[ffff]", &x, &y, &w, &h);

  float32 width = world.Metres(w);
  float32 height = world.Metres(h);

  ShapePair shape;
  shape.first = Shape(new b2PolygonShape);
  b2PolygonShape& box = *(b2PolygonShape*)shape.first.get();
  box.SetAsBox(.5f * width, .5f * height, b2Vec2(world.Metres(x), world.Metres(y)), 0.f);

  shape.second = width * height;
  return shape;
}

static ShapePair Circle(WorldImpl const& world, json::JSON& json)
{
  double x, y, radius;
  json.Unpack("[fff]", &x, &y, &radius);

  ShapePair shape;
  shape.first = Shape(new b2CircleShape);
  b2CircleShape& circle = *(b2CircleShape*)shape.first.get();
  circle.m_p.Set(world.Metres(x), world.Metres(y));
  circle.m_radius = world.Metres(radius);

  shape.second = circle.m_radius * circle.m_radius * b2_pi;
  return shape;
}

static ShapePair Chain(WorldImpl const& world, json::JSON& json)
{
  std::vector<b2Vec2> vertices;
  for(json::JSON const& value : json)
  {
    double x, y;
    value.Unpack("[ff]", &x, &y);
    vertices.emplace_back(world.Metres(x), world.Metres(y));
  }

  ShapePair shape;
  shape.first = Shape(new b2ChainShape);
  b2ChainShape& chain = *(b2ChainShape*)shape.first.get();

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
  shape.second = 0.f;
  return shape;
}

static b2BodyDef BodyDefinition(WorldImpl const& world, json::JSON& velocity, float32 damping, float32 x, float32 y)
{
  b2BodyDef body_def;
  if(velocity)
  {
    body_def.type = b2_dynamicBody;
    double u, v;
    velocity.Unpack("[ff]", &u, &v);
    body_def.linearVelocity.Set(world.Metres(u), world.Metres(v));
  }
  else
  {
    body_def.type = b2_staticBody;
    body_def.linearVelocity.SetZero();
  }
  body_def.position.Set(x, y);
  body_def.angularVelocity = 0.f;
  body_def.linearDamping = damping;
  body_def.angularDamping = 0.f;
  body_def.fixedRotation = true;
  return body_def;
}

static b2FixtureDef FixtureDefinition(float32 area, float32 mass, float32 friction, float32 restitution)
{
  b2FixtureDef fixture;
  float32 density = 0.f;
  if(area > 0.f)
  {
    density = mass / area;
  }
  fixture.density = density;
  if(mass == 0.f)
  {
    fixture.isSensor = true;
    fixture.friction = 0.f;
    fixture.restitution = 0.f;
  }
  else
  {
    fixture.isSensor = false;
    fixture.friction = friction;
    fixture.restitution = restitution;
  }
  return fixture;
}

BodyImpl::BodyImpl(json::JSON const& json, World& world)
{
  json_t* shapes_ref;
  json_t* light;
  json_t* velocity;
  double x, y, m, c, d, k;
 
  json.Unpack("{s[ff]sososfsfsfsfso}",
    "position", &x, &y,
    "velocity", &velocity,
    "shapes", &shapes_ref,
    "mass", &m,
    "restitution", &c,
    "drag", &d,
    "friction", &k,
    "light", &light);

  float32 area = 0.f;
  std::vector<Shape> shapes(json::JSON(shapes_ref).Size());

  auto shape = shapes.begin();
  for(json::JSON const& fixture_ref : json::JSON(shapes_ref))
  {
    char const* type_ref;
    json_t* shape_ref;
    fixture_ref.Unpack("{ssso}",
      "type", &type_ref,
      "shape", &shape_ref);

    std::string type(type_ref);
    if(type == "box")
    {
      float32 temp;
      std::tie(*shape, temp) = Box(*world.impl_, json::JSON(shape_ref));
      area += temp;
    }
    else if(type == "circle")
    {
      float32 temp;
      std::tie(*shape, temp) = Circle(*world.impl_, json::JSON(shape_ref));
      area += temp;
    }
    else if(type == "chain")
    {
      float32 temp;
      std::tie(*shape, temp) = Chain(*world.impl_, json::JSON(shape_ref));
      area += temp;
    }
    else
    {
      BOOST_THROW_EXCEPTION(exception::Exception());
    }
    ++shape;
  }

  b2BodyDef body_def = BodyDefinition(*world.impl_, json::JSON(velocity), float32(d), world.impl_->Metres(x), world.impl_->Metres(y));
  body_ = world.impl_->world_.CreateBody(&body_def);
  body_->SetUserData(this);

  b2FixtureDef fixture = FixtureDefinition(area, float32(m), float32(k), float32(c));

  for(auto& shape : shapes)
  {
    fixture.shape = shape.get();
    (void)body_->CreateFixture(&fixture);
  }

  body_->ResetMassData();

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
  auto world = world_.Lock().impl_;
  return game::Position(world->Pixels(position_.x), world->Pixels(position_.y));
}

void BodyImpl::Position(float x, float y)
{
  auto world = world_.Lock().impl_;
  body_->SetTransform(b2Vec2(world->Metres(x), world->Metres(y)), 0.f);
}

game::Position BodyImpl::Velocity(void) const
{
  auto world = world_.Lock().impl_;
  return game::Position(world->Pixels(velocity_.x), world->Pixels(velocity_.y));
}

void BodyImpl::Velocity(float x, float y)
{
  auto world = world_.Lock().impl_;
  body_->SetLinearVelocity(b2Vec2(world->Metres(x), world->Metres(y)));
}

void BodyImpl::Force(float x, float y)
{
  auto world = world_.Lock().impl_;
  body_->ApplyForceToCenter(b2Vec2(world->Metres(x), world->Metres(y)), true);
}

void BodyImpl::Impulse(float x, float y)
{
  auto world = world_.Lock().impl_;
  body_->ApplyLinearImpulse(b2Vec2(world->Metres(x), world->Metres(y)), body_->GetWorldCenter(), true);
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
  if(world_.Lock())
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