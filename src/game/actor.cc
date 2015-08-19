#include "actor.h"
#include "bounding_box.h"
#include "bind.h"
#include <random>
#include "actor_impl.h"
namespace
{
auto interval = std::bind(std::uniform_real_distribution<double>(2., 20.), std::default_random_engine());

dynamics::Body MakeBody(lua::Stack& lua, dynamics::World& world, collision::Group& collision)
{
  dynamics::Body body;
  {
    lua::Guard guard = lua.Field("body");
    body = dynamics::Body(lua, world);
  }

  {
    lua::Guard guard = lua.Field("names");
    for(int index = 1, end = lua.Size(); index <= end; ++index)
    {
      collision.Link(lua.Field<std::string>(index), body);
    }
  }
  return body;
}
}

namespace game
{
Actor::Impl::~Impl()
{
  collision_.Unlink(dynamics_body_);
}

auto Actor::Impl::Pause() -> void
{
  if(animation_)
  {
    animation_.Pause();
  }
  if(blink_)
  {
    blink_.Pause();
  }
}

auto Actor::Impl::Resume() -> void
{
  if(animation_)
  {
    animation_.Resume();
  }
  if(blink_)
  {
    blink_.Resume();
  }
}

auto Actor::Impl::Modulation(float r, float g, float b, float a) -> void
{
  modulation_.r(r);
  modulation_.g(g);
  modulation_.b(b);
  modulation_.a(a);

  if(game_body_)
  {
    if(dynamics_body_)
    {
      display::Modulation modulation = dynamics_body_.Modulation();
      game_body_.Modulation(modulation.r() * r, modulation.g() * g, modulation.b() * b, modulation.a() * a);
    }
    else
    {
      game_body_.Modulation(r, g, b, a);
    }
  }
}

auto Actor::Impl::Begin() -> void
{
  dynamics_body_.Force(force_.first, force_.second);
}

auto Actor::Impl::End() -> void
{
  game::Position position(dynamics_body_.Position());
  position.first += position_.first;
  position.second += position_.second;
  game_body_.Position(position, angle_ + dynamics_body_.Rotation());
 
  display::Modulation modulation(dynamics_body_.Modulation());
  game_body_.Modulation(modulation.r() * modulation_.r(), modulation.g() * modulation_.g(), modulation.b() * modulation_.b(), modulation.a() * modulation_.a());
}

auto Actor::Impl::Render() const -> void
{
  if(game_body_)
  {
    game_body_.Render();
  }
}

auto Actor::Impl::Position(game::Position const& position) -> void
{
  if(dynamics_body_)
  {
    dynamics_body_.Position(position.first, position.second);
  }
  if(game_body_)
  {
    game::Position temp = position;
    if(dynamics_body_)
    {
      temp.first += position_.first;
      temp.second += position_.second;
    }
    game_body_.Position(temp);
  }
}

auto Actor::Impl::Position() -> game::Position
{
  game::Position position;
  if(dynamics_body_)
  {
    position = dynamics_body_.Position();
  }
  else if(game_body_)
  {
    position = game_body_.Position();
  }
  return position;
}

auto Actor::Impl::Velocity(game::Position const& velocity) -> void
{
  if(dynamics_body_)
  {
    dynamics_body_.Velocity(velocity.first, velocity.second);
  }
}

auto Actor::Impl::Velocity() const -> game::Position
{
  game::Position velocity;
  if(dynamics_body_)
  {
    velocity = dynamics_body_.Velocity();
  }
  return velocity;
}

auto Actor::Impl::Force(game::Position const& force) -> void
{
  force_ = force;
}

auto Actor::Impl::Impulse(game::Position const& impulse) -> void
{
  if(dynamics_body_)
  {
    dynamics_body_.Impulse(impulse.first, impulse.second);
  }
}

auto Actor::Impl::Body(std::string const& expression, bool left_facing) -> void
{
  if(game_body_)
  {
    animation_.Reset(dilation_ * game_body_.Expression(expression, left_facing), -1);
  }
}

auto Actor::Impl::Body(std::string const& expression) -> void
{
  if(game_body_)
  {
    animation_.Reset(dilation_ * game_body_.Expression(expression), -1);
  }
}

auto Actor::Impl::Body(bool left_facing) -> void
{
  if(game_body_)
  {
    animation_.Reset(dilation_ * game_body_.Expression(left_facing), -1);
  }
}

auto Actor::Impl::Eyes(std::string const& expression) -> void
{
  if(eyes_)
  {
    eyes_.Expression(expression);
  }
}

auto Actor::Impl::Mouth(std::string const& expression) -> void
{
  if(mouth_)
  {
    mouth_.Expression(expression);
  }
}

auto Actor::Impl::Mouth(int open) -> void
{
  if(mouth_)
  {
    mouth_.Expression(open);
  }
}

auto Actor::Impl::Blink() -> void
{
  double t = .2;
  if(!open_)
  {
    t = interval();
  }
  t *= dilation_;

  blink_.Reset(t, -1);

  open_ ^= true;
  eyes_.Expression(open_);
}

Actor::Impl::Impl(lua::Stack& lua, display::Window& window, event::Queue& queue, dynamics::World& world, collision::Group& collision, boost::filesystem::path const& path) : force_(0.f, 0.f), open_(true), angle_(0.), collision_(collision)
{
  {
    lua::Guard guard = lua.Field("dynamics_body");
    if(lua.Check())
    {
      dynamics_body_ = MakeBody(lua, world, collision_);
    }
  }

  {
    lua::Guard guard = lua.Field("game_body");
    if(lua.Check())
    {
      {
        lua::Guard guard = lua.Field("dilation");
        if(lua.Check())
        {
          lua.Pop(dilation_);
        }
        else
        {
          dilation_ = 1.;
        }
      }

      {
        lua::Guard guard = lua.Field("eyes");
        if(lua.Check())
        {
          eyes_ = game::Feature(lua, window, path);
          
          lua::Guard guard = lua.Field("blink");
          if(lua.Check())
          {
            bool blink;
            lua.Pop(blink);
            if(blink)
            {
              blink_ = event::Timer(interval() * dilation_, -1);
              queue.Add(function::Bind(&event::Timer::operator(), blink_));
            }
          }
        }
      }

      {
        lua::Guard guard = lua.Field("mouth");
        if(lua.Check())
        {
          mouth_ = game::Feature(lua, window, path);
        }
      }

      game_body_ = game::Body(lua, window, path, eyes_, mouth_);
    
      animation_ = event::Timer(dilation_ * game_body_.Period(), -1);
      queue.Add(function::Bind(&event::Timer::operator(), animation_));
    }
  }
  
  if(bool(dynamics_body_) && bool(game_body_))
  {
    modulation_ = game_body_.Modulation();

    game::Position dynamics_position = dynamics_body_.Position();
    game::Position game_position = game_body_.Position();

    position_.first = game_position.first - dynamics_position.first;
    position_.second = game_position.second - dynamics_position.second;
    angle_ = game_body_.Rotation() - dynamics_body_.Rotation();
  }
}

auto Actor::Impl::Init(dynamics::World& world) -> void
{
  if(dynamics_body_)
  {
    world.Begin(function::Bind(&Impl::Begin, shared_from_this()));
    if(game_body_)
    {
      world.End(function::Bind(&Impl::End, shared_from_this()));
    }
  }
  if(blink_)
  {
    blink_.Add(function::Bind(&Impl::Blink, shared_from_this()));
  }
  if(animation_)
  {
    animation_.Add(function::Bind(&Impl::Next, shared_from_this()));
  }
}

auto Actor::Impl::Next() -> void
{
  animation_.Reset(dilation_ * game_body_.Next(), -1);
}

auto Actor::Impl::Dilation() const -> double
{
  return dilation_;
}

auto Actor::Impl::Dilation(double dilation) -> void
{
  dilation_ = dilation;

  if(blink_)
  {
    double t = .2;
    if(!open_)
    {
      t = interval();
    }
    t *= dilation_;
    blink_.Reset(t, -1);
  }

  if(animation_)
  {
    animation_.Reset(dilation_ * game_body_.Period(), -1);
  }
}

auto Actor::Impl::Rotation() const -> double
{
  double angle = 0.;
  if(dynamics_body_)
  {
    angle = dynamics_body_.Rotation();
  }
  else if(game_body_)
  {
    angle = game_body_.Rotation();
  }
  return angle;
}

auto Actor::Impl::Rotation(double angle) -> void
{
  if(dynamics_body_)
  {
    dynamics_body_.Rotation(angle);
  }
  if(game_body_)
  {
    game_body_.Rotation(angle + angle_);
  }
}

auto Actor::Impl::Scale(float scale) -> void
{
  if(game_body_)
  {
    game_body_.Scale(scale);
  }
}

auto Actor::Impl::Scale() const -> float
{
  float scale = 0.f;
  if(game_body_)
  {
    scale = game_body_.Scale();
  }
  return scale;
}

auto Actor::Impl::Link(std::string const& group) -> void
{
  collision_.Link(group, dynamics_body_);
}

auto Actor::Impl::Unlink(std::string const& group) -> void
{
  collision_.Unlink(group, dynamics_body_);
}

auto Actor::Position(game::Position const& position) -> void
{
  impl_->Position(position);
}

auto Actor::Position() const -> game::Position
{
  return impl_->Position();
}

auto Actor::Velocity(game::Position const& velocity) -> void
{
  impl_->Velocity(velocity);
}

auto Actor::Velocity() const -> game::Position
{
  return impl_->Velocity();
}

auto Actor::Force(game::Position const& force) -> void
{
  impl_->Force(force);
}

auto Actor::Impulse(game::Position const& impulse) -> void
{
  impl_->Impulse(impulse);
}

auto Actor::Modulation(float r, float g, float b, float a) -> void
{
  impl_->Modulation(r, g, b, a);
}

auto Actor::Pause() -> void
{
  impl_->Pause();
}

auto Actor::Resume() -> void
{
  impl_->Resume();
}

auto Actor::Body(std::string const& expression, bool left_facing) -> void
{
  impl_->Body(expression, left_facing);
}

auto Actor::Body(std::string const& expression) -> void
{
  impl_->Body(expression);
}

auto Actor::Body(bool left_facing) -> void
{
  impl_->Body(left_facing);
}

auto Actor::Eyes(std::string const& expression) -> void
{
  impl_->Eyes(expression);
}

auto Actor::Mouth(std::string const& expression) -> void
{
  impl_->Mouth(expression);
}

auto Actor::Mouth(int open) -> void
{
  impl_->Mouth(open);
}

auto Actor::Dilation() const -> double
{
  return impl_->Dilation();
}

auto Actor::Dilation(double dilation) -> void
{
  impl_->Dilation(dilation);
}

Actor::operator bool() const
{
  return bool(impl_);
}

auto Actor::Rotation(double angle) -> void
{
  impl_->Rotation(angle);
}
  
auto Actor::Rotation() const -> double
{
  return impl_->Rotation();
}

auto Actor::Scale(float scale) -> void
{
  impl_->Scale(scale);
}

auto Actor::Scale() const -> float
{
  return impl_->Scale();
}

auto Actor::Render() const -> void
{
  impl_->Render();
}

Actor::Actor(lua::Stack& lua, display::Window& window, collision::Group& collision, event::Queue& queue, dynamics::World& world, boost::filesystem::path const& path)
{
  impl_ = std::make_shared<Impl>(lua, window, queue, world, collision, path);
  impl_->Init(world);
}

size_t Actor::Hash(Actor const& actor)
{
  return std::hash<std::shared_ptr<Impl>>()(actor.impl_);
}

bool Actor::operator==(Actor const& other) const
{
  return impl_ == other.impl_;
}

auto Actor::Link(std::string const& group) -> void
{
  impl_->Link(group);
}

auto Actor::Unlink(std::string const& group) -> void
{
  impl_->Unlink(group);
}
}