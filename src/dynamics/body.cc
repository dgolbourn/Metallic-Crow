#include "world_impl.h"
#include "body_impl.h"
#include <vector>
#include "exception.h"
#include "log.h"
namespace 
{
typedef std::unique_ptr<b2Shape> Shape;
typedef std::pair<Shape, float32> ShapePair;

auto Box(dynamics::WorldImpl const& world, lua::Stack& lua) -> ShapePair
{
  double x;
  {
    lua::Guard guard = lua.Field(1);
    lua.Pop(x);
  }

  double y;
  {
    lua::Guard guard = lua.Field(2);
    lua.Pop(y);
  }

  double w;
  {
    lua::Guard guard = lua.Field(3);
    lua.Pop(w);
  }

  double h;
  {
    lua::Guard guard = lua.Field(4);
    lua.Pop(h);
  }

  float32 width = world.Metres(w);
  float32 height = world.Metres(h);

  ShapePair shape;
  shape.first = Shape(new b2PolygonShape);
  b2PolygonShape& box = *dynamic_cast<b2PolygonShape*>(shape.first.get());
  box.SetAsBox(.5f * width, .5f * height, b2Vec2(world.Metres(x), world.Metres(y)), 0.f);

  shape.second = width * height;
  return shape;
}

auto Circle(dynamics::WorldImpl const& world, lua::Stack& lua) -> ShapePair
{
  double x;
  {
    lua::Guard guard = lua.Field(1);
    lua.Pop(x);
  }

  double y;
  {
    lua::Guard guard = lua.Field(2);
    lua.Pop(y);
  }

  double radius;
  {
    lua::Guard guard = lua.Field(3);
    lua.Pop(radius);
  }

  ShapePair shape;
  shape.first = Shape(new b2CircleShape);
  b2CircleShape& circle = *dynamic_cast<b2CircleShape*>(shape.first.get());
  circle.m_p.Set(world.Metres(x), world.Metres(y));
  circle.m_radius = world.Metres(radius);

  shape.second = circle.m_radius * circle.m_radius * b2_pi;
  return shape;
}

auto Chain(dynamics::WorldImpl const& world, lua::Stack& lua) -> ShapePair
{
  std::vector<b2Vec2> vertices;
  for(int index = 1, end = lua.Size(); index <= end; ++index)
  {
    lua::Guard guard = lua.Field(index);
   
    double x;
    {
      lua::Guard guard = lua.Field(1);
      lua.Pop(x);
    }

    double y;
    {
      lua::Guard guard = lua.Field(2);
      lua.Pop(y);
    }

    vertices.emplace_back(world.Metres(x), world.Metres(y));
  }

  ShapePair shape;
  shape.first = Shape(new b2ChainShape);
  b2ChainShape& chain = *dynamic_cast<b2ChainShape*>(shape.first.get());

  if(vertices.front() == vertices.back())
  {
    vertices.pop_back();
    chain.CreateLoop(vertices.data(), vertices.size());
  }
  else
  {
    chain.CreateChain(vertices.data(), vertices.size());
  }
  shape.second = 0.f;
  return shape;
}

auto BodyDefinition(dynamics::WorldImpl const& world, lua::Stack& lua, float32 damping, float32 x, float32 y) -> b2BodyDef
{
  b2BodyDef body_def;
 
  lua::Guard guard = lua.Field("velocity");
  if(lua.Check())
  {
    double u;
    {
      lua::Guard guard = lua.Field(1);
      lua.Pop(u);
    }

    double v;
    {
      lua::Guard guard = lua.Field(2);
      lua.Pop(v);
    }

    body_def.type = b2_dynamicBody;
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

auto FixtureDefinition(float32 area, float32 mass, float32 friction, float32 restitution) -> b2FixtureDef
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
}

namespace dynamics
{
BodyImpl::BodyImpl(lua::Stack& lua, World& world)
{
  float32 area = 0.f;
  std::vector<Shape> shapes;

  {
    lua::Guard guard = lua.Field("shapes");
    int size = lua.Size();
    shapes.reserve(size);

    for(int index = 1; index <= size; ++index)
    {
      lua::Guard guard = lua.Field(index);

      std::string type;
      {
        lua::Guard guard = lua.Field("type");
        lua.Pop(type);
      }

      Shape shape;
      if(type == "box")
      {
        lua::Guard guard = lua.Field("shape");
        float32 temp;
        std::tie(shape, temp) = Box(*world.impl_, lua);
        area += temp;
      }
      else if(type == "circle")
      {
        lua::Guard guard = lua.Field("shape");
        float32 temp;
        std::tie(shape, temp) = Circle(*world.impl_, lua);
        area += temp;
      }
      else if(type == "chain")
      {
        lua::Guard guard = lua.Field("shape");
        float32 temp;
        std::tie(shape, temp) = Chain(*world.impl_, lua);
        area += temp;
      }
      else
      {
        BOOST_THROW_EXCEPTION(exception::Exception());
      }
      
      shapes.emplace_back(std::move(shape));
    }
  }

  float32 drag;
  {
    lua::Guard guard = lua.Field("drag");
    lua.Pop(drag);
  }

  double x, y;
  {
    lua::Guard guard = lua.Field("position");
    {
      lua::Guard guard = lua.Field(1);
      lua.Pop(x);
    }

    {
      lua::Guard guard = lua.Field(2);
      lua.Pop(y);
    }
  }

  b2BodyDef body_def = BodyDefinition(*world.impl_, lua, drag, world.impl_->Metres(x), world.impl_->Metres(y));

  body_ = world.impl_->world_.CreateBody(&body_def);
  body_->SetUserData(this);

  float32 mass;
  {
    lua::Guard guard = lua.Field("mass");
    lua.Pop(mass);
  }

  float32 restitution;
  {
    lua::Guard guard = lua.Field("restitution");
    lua.Pop(restitution);
  }

  float32 friction;
  {
    lua::Guard guard = lua.Field("friction");
    lua.Pop(friction);
  }

  b2FixtureDef fixture = FixtureDefinition(area, mass, friction, restitution);

  for(auto& shape : shapes)
  {
    fixture.shape = shape.get();
    body_->CreateFixture(&fixture);
  }

  body_->ResetMassData();

  world_ = world;
  position_ = body_->GetPosition();
  velocity_ = body_->GetLinearVelocity();

  {
    lua::Guard guard = lua.Field("light");
    if(lua.Check())
    {
      light_ = Light(lua);
    }
  }
}

auto BodyImpl::MakeBody(b2Body* body_ptr) -> Body
{
  Body body;
  if(body_ptr)
  {
    BodyImpl* impl = static_cast<BodyImpl*>(body_ptr->GetUserData());
    if(impl)
    {
      body.impl_ = impl->shared_from_this();
    }
  }
  return body;
}

auto BodyImpl::Position() const -> game::Position 
{
  auto world = world_.Lock().impl_;
  return game::Position(world->Pixels(position_.x), world->Pixels(position_.y));
}

auto BodyImpl::Position(float x, float y) -> void
{
  auto world = world_.Lock().impl_;
  body_->SetTransform(b2Vec2(world->Metres(x), world->Metres(y)), 0.f);
}

auto BodyImpl::Velocity() const -> game::Position
{
  auto world = world_.Lock().impl_;
  return game::Position(world->Pixels(velocity_.x), world->Pixels(velocity_.y));
}

auto BodyImpl::Velocity(float x, float y) -> void
{
  auto world = world_.Lock().impl_;
  body_->SetLinearVelocity(b2Vec2(world->Metres(x), world->Metres(y)));
}

auto BodyImpl::Force(float x, float y) -> void
{
  auto world = world_.Lock().impl_;
  body_->ApplyForceToCenter(b2Vec2(world->Metres(x), world->Metres(y)), true);
}

auto BodyImpl::Impulse(float x, float y) -> void
{
  auto world = world_.Lock().impl_;
  body_->ApplyLinearImpulse(b2Vec2(world->Metres(x), world->Metres(y)), body_->GetWorldCenter(), true);
}

auto BodyImpl::Begin() -> void
{
  if(body_->GetType() != b2_staticBody)
  {
    position_ = body_->GetPosition();
    velocity_ = body_->GetLinearVelocity();
  }
}

auto BodyImpl::End(float32 dt) -> void
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

auto BodyImpl::Update(float32 ds) -> void
{
  if(body_->GetType() != b2_staticBody)
  {
    position_ = ds * (ds * (ds * cubic_[3] + cubic_[2]) + cubic_[1]) + cubic_[0];
    velocity_ = ds * (ds * 3.f * cubic_[3] + 2.f * cubic_[2]) + cubic_[1];
  }
}

auto BodyImpl::Modulation() const -> display::Modulation
{
  return display::Modulation(light_.illumination.x, light_.illumination.y, light_.illumination.z, 1.f);
}

BodyImpl::~BodyImpl()
{
  try
  { 
    if(world_.Lock())
    {
      body_->SetUserData(nullptr);
      body_->GetWorld()->DestroyBody(body_);
    }
  }
  catch(...)
  {
    exception::Log("Swallowed exception");
  }
}

auto Body::operator<(Body const& other) const -> bool
{
  return impl_.owner_before(other.impl_);
}

Body::operator bool() const
{
  return bool(impl_) && bool(impl_->world_.Lock());
}

auto Body::Position() const -> game::Position
{
  return impl_->Position();
}

auto Body::Position(float x, float y) -> void
{
  impl_->Position(x, y);
}

auto Body::Velocity() const -> game::Position
{
  return impl_->Velocity();
}

auto Body::Velocity(float x, float y) -> void
{
  impl_->Velocity(x, y);
}

auto Body::Force(float x, float y) -> void
{
  impl_->Force(x, y);
}

auto Body::Impulse(float x, float y) -> void
{
  impl_->Impulse(x, y);
}

auto Body::Modulation() const -> display::Modulation
{
  return impl_->Modulation();
}

Body::Body(lua::Stack& lua, World& world) : impl_(std::make_shared<BodyImpl>(lua, world))
{
}
}