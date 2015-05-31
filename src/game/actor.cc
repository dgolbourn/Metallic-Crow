#include "actor.h"
#include "bounding_box.h"
#include "bind.h"
#include "json_iterator.h"
#include <random>
#include "make_body.h"
#include "actor_impl.h"
namespace
{
auto interval = std::bind(std::uniform_real_distribution<double>(2., 20.), std::default_random_engine());
}

namespace game
{
void Actor::Impl::Pause()
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

void Actor::Impl::Resume()
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

void Actor::Impl::Modulation(float r, float g, float b, float a)
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

void Actor::Impl::Begin()
{
  dynamics_body_.Force(force_.first, force_.second);
}

void Actor::Impl::End()
{
  game::Position position(dynamics_body_.Position());
  position.first += position_.first;
  position.second += position_.second;
  game_body_.Position(position);

  display::Modulation modulation(dynamics_body_.Modulation());
  game_body_.Modulation(modulation.r() * modulation_.r(), modulation.g() * modulation_.g(), modulation.b() * modulation_.b(), modulation.a() * modulation_.a());
}

void Actor::Impl::Render()
{
  game_body_.Render();
}

void Actor::Impl::Position(game::Position const& position)
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

game::Position Actor::Impl::Position()
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

void Actor::Impl::Velocity(game::Position const& velocity)
{
  if(dynamics_body_)
  {
    dynamics_body_.Velocity(velocity.first, velocity.second);
  }
}

game::Position Actor::Impl::Velocity() const
{
  game::Position velocity;
  if(dynamics_body_)
  {
    velocity = dynamics_body_.Velocity();
  }
  return velocity;
}

void Actor::Impl::Force(game::Position const& force)
{
  force_ = force;
}

void Actor::Impl::Impulse(game::Position const& impulse)
{
  if(dynamics_body_)
  {
    dynamics_body_.Impulse(impulse.first, impulse.second);
  }
}

void Actor::Impl::Body(std::string const& expression, bool left_facing)
{
  if(game_body_)
  {
    animation_.Reset(dilation_ * game_body_.Expression(expression, left_facing), -1);
  }
}

void Actor::Impl::Body(std::string const& expression)
{
  if(game_body_)
  {
    animation_.Reset(dilation_ * game_body_.Expression(expression), -1);
  }
}

void Actor::Impl::Body(bool left_facing)
{
  if(game_body_)
  {
    animation_.Reset(dilation_ * game_body_.Expression(left_facing), -1);
  }
}

void Actor::Impl::Eyes(std::string const& expression)
{
  if(eyes_)
  {
    eyes_.Expression(expression);
  }
}

void Actor::Impl::Mouth(std::string const& expression)
{
  if(mouth_)
  {
    mouth_.Expression(expression);
  }
}

void Actor::Impl::Mouth(int open)
{
  if(mouth_)
  {
    mouth_.Expression(open);
  }
}

void Actor::Impl::Blink()
{
  double t = .2;
  if(!open_)
  {
    t = interval();
  }

  blink_.Reset(t, -1);

  open_ ^= true;
  eyes_.Expression(open_);
}

Actor::Impl::Impl(json::JSON const& json, display::Window& window, event::Queue& queue, dynamics::World& world, collision::Group& collision, int& plane, boost::filesystem::path const& path) : force_(0.f, 0.f), open_(true)
{
  json_t* dynamics_body;
  json_t* game_body;
  json_t* eyes;
  json_t* mouth;
  json_t* blink;
  json_t* plane_ref;
  json_t* dilation;
  json.Unpack("{sososososososo}",
    "dynamics body", &dynamics_body,
    "dilation", &dilation,
    "game body", &game_body,
    "eyes", &eyes,
    "mouth", &mouth,
    "plane", &plane_ref,
    "blink", &blink);

  dynamics_body_ = MakeBody(json::JSON(dynamics_body), world, collision);
  if(json::JSON json = json::JSON(game_body))
  {
    if(json::JSON json = json::JSON(eyes))
    {
      eyes_ = game::Feature(json, window, path);
    }
    if(json::JSON json = json::JSON(mouth))
    {
      mouth_ = game::Feature(json, window, path);
    }
  
    game_body_ = game::Body(json, window, path, eyes_, mouth_);

    json::JSON(dilation).Unpack("f", &dilation_);
    animation_ = event::Timer(dilation_ * game_body_.Period(), -1);
    queue.Add(function::Bind(&event::Timer::operator(), animation_));

    json::JSON(plane_ref).Unpack("i", &plane);

    if(json::JSON json = json::JSON(blink))
    {
      int bblink;
      json.Unpack("b", &bblink);
      if((bblink != 0) && bool(eyes_))
      {
        blink_ = event::Timer(interval(), -1);
        queue.Add(function::Bind(&event::Timer::operator(), blink_));
      }
    }
  }

  if(bool(dynamics_body_) && bool(game_body_))
  {
    modulation_ = game_body_.Modulation();

    game::Position dynamics_position = dynamics_body_.Position();
    game::Position game_position = game_body_.Position();

    position_.first = game_position.first - dynamics_position.first;
    position_.second = game_position.second - dynamics_position.second;
  }
}

void Actor::Impl::Init(Scene& scene, dynamics::World& world, int plane)
{
  if(dynamics_body_)
  {
    world.Begin(function::Bind(&Impl::Begin, shared_from_this()));
    if(game_body_)
    {
      world.End(function::Bind(&Impl::End, shared_from_this()));
    }
  }
  if(game_body_)
  {
    scene.Add(function::Bind(&Impl::Render, shared_from_this()), plane);
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

void Actor::Impl::Next()
{
  animation_.Reset(dilation_ * game_body_.Next(), -1);
}

double Actor::Impl::Dilation() const
{
  return dilation_;
}

void Actor::Impl::Dilation(double dilation)
{
  dilation_ = dilation;
}

void Actor::Position(game::Position const& position)
{
  impl_->Position(position);
}

game::Position Actor::Position() const
{
  return impl_->Position();
}

void Actor::Velocity(game::Position const& velocity)
{
  impl_->Velocity(velocity);
}

game::Position Actor::Velocity() const
{
  return impl_->Velocity();
}

void Actor::Force(game::Position const& force)
{
  impl_->Force(force);
}

void Actor::Impulse(game::Position const& impulse)
{
  impl_->Impulse(impulse);
}

void Actor::Modulation(float r, float g, float b, float a)
{
  impl_->Modulation(r, g, b, a);
}

void Actor::Pause()
{
  impl_->Pause();
}

void Actor::Resume()
{
  impl_->Resume();
}

void Actor::Body(std::string const& expression, bool left_facing)
{
  impl_->Body(expression, left_facing);
}

void Actor::Body(std::string const& expression)
{
  impl_->Body(expression);
}

void Actor::Body(bool left_facing)
{
  impl_->Body(left_facing);
}

void Actor::Eyes(std::string const& expression)
{
  impl_->Eyes(expression);
}

void Actor::Mouth(std::string const& expression)
{
  impl_->Mouth(expression);
}

void Actor::Mouth(int open)
{
  impl_->Mouth(open);
}

double Actor::Dilation() const
{
  return impl_->Dilation();
}

void Actor::Dilation(double dilation)
{
  impl_->Dilation(dilation);
}

Actor::operator bool() const
{
  return bool(impl_);
}

Actor::Actor(json::JSON const& json, display::Window& window, Scene& scene, collision::Group& collision, event::Queue& queue, dynamics::World& world, boost::filesystem::path const& path)
{
  int plane;
  impl_ = std::make_shared<Impl>(json, window, queue, world, collision, plane, path);
  impl_->Init(scene, world, plane);
}
}