#include "actor.h"
#include "bounding_box.h"
#include "bind.h"
#include "avatar.h"
#include "json_iterator.h"
#include "timer.h"
#include <random>
#include "make_body.h"
namespace
{
auto interval = std::bind(std::uniform_real_distribution<double>(2., 20.), std::default_random_engine());
}

namespace game
{
class Actor::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(json::JSON const& json, display::Window& window, event::Queue& queue, dynamics::World& world, collision::Group& collision, int& plane, boost::filesystem::path const& path);
  void Init(Scene& scene, dynamics::World& world, int plane);
  void Render(void) const;
  void Pause(void);
  void Resume(void);
  void Up(void);
  void Down(void);
  void Left(void);
  void Right(void);
  void Body(std::string const& expression);
  void Eyes(std::string const& expression);
  void Mouth(std::string const& expression);
  void Mouth(int open);
  void Update(void);
  void Position(game::Position const& position);
  game::Position Position(void) const;
  void Velocity(game::Position const& velocity);
  game::Position Velocity(void) const;
  void Force(game::Position const& force);
  void Impulse(game::Position const& impulse);
  void Begin(void);
  void End(void);
  void Blink(void);

  Avatar avatar_;
  dynamics::Body body_;
  
  bool paused_;
  int x_sign_;
  int y_sign_;
  game::Position force_;
  float thrust_;

  event::Timer blink_;
  bool open_;
};

void Actor::Impl::Pause(void)
{
  paused_ = true;
  avatar_.Pause();
  blink_.Pause();
}

void Actor::Impl::Resume(void)
{
  paused_ = false;
  avatar_.Resume();
  blink_.Resume();
}

void Actor::Impl::Up(void)
{
  if(!paused_)
  {
    --y_sign_;
    Update();
  }
}

void Actor::Impl::Down(void)
{
  if(!paused_)
  {
    ++y_sign_;
    Update();
  }
}

void Actor::Impl::Left(void)
{
  if(!paused_)
  {
    --x_sign_;
    Update();
  }
}

void Actor::Impl::Right(void)
{
  if(!paused_)
  {
    ++x_sign_;
    Update();
  }
}

void Actor::Impl::Update(void)
{
  float force = thrust_;
  if(x_sign_ && y_sign_)
  {
    force *= std::sqrt(0.5f);
  }
  force_.first = float(x_sign_) * force;
  force_.second = float(y_sign_) * force;

  if(x_sign_ < 0)
  {
    avatar_.Facing(true);
  }
  else if(x_sign_ > 0)
  {
    avatar_.Facing(false);
  }

  if(x_sign_ || y_sign_)
  {
    avatar_.Body("active");
  }
  else
  {
    avatar_.Body("idle");
  }
}

void Actor::Impl::Begin(void)
{
  body_.Force(force_.first, force_.second);
}

void Actor::Impl::End(void)
{
  avatar_.Position(body_.Position());
  avatar_.Modulation(body_.Modulation());
}

void Actor::Impl::Render(void) const
{
  avatar_.Render();
}

void Actor::Impl::Position(game::Position const& position)
{
  body_.Position(position.first, position.second);
}

game::Position Actor::Impl::Position(void) const
{
  return body_.Position();
}

void Actor::Impl::Velocity(game::Position const& velocity)
{
  body_.Velocity(velocity.first, velocity.second);
}

game::Position Actor::Impl::Velocity(void) const
{
  return body_.Velocity();
}

void Actor::Impl::Force(game::Position const& force)
{
  force_ = force;
}

void Actor::Impl::Impulse(game::Position const& impulse)
{
  body_.Impulse(impulse.first, impulse.second);
}

void Actor::Impl::Body(std::string const& expression)
{
  avatar_.Body(expression);
}

void Actor::Impl::Eyes(std::string const& expression)
{
  avatar_.Eyes(expression);
}

void Actor::Impl::Mouth(std::string const& expression)
{
  avatar_.Mouth(expression);
}

void Actor::Impl::Mouth(int open)
{
  avatar_.Mouth(open);
}

void Actor::Impl::Blink()
{
  double t = .2;
  if(!open_)
  {
    t = interval();
  }

  blink_.Reset(t, 0);
  if(!paused_)
  {
    blink_.Resume();
  }

  open_ ^= true;
  avatar_.Eyes(open_);
}

Actor::Impl::Impl(json::JSON const& json, display::Window& window, event::Queue& queue, dynamics::World& world, collision::Group& collision, int& plane, boost::filesystem::path const& path) :
  paused_(true), 
  x_sign_(0), 
  y_sign_(0), 
  force_(0.f, 0.f), 
  thrust_(2000.f),
  blink_(interval(), 0),
  open_(true)
{
  json_t* avatar;
  json_t* body;
  json.Unpack("{sososi}",
    "body", &body,
    "avatar", &avatar,
    "plane", &plane);

  body_ = MakeBody(json::JSON(body), world, collision);

  avatar_ = Avatar(json::JSON(avatar), window, queue, path);
  avatar_.Body("idle");
  avatar_.Eyes("idle");
  avatar_.Eyes(open_);
  avatar_.Mouth("idle");

  queue.Add(function::Bind(&event::Timer::operator(), blink_));
}

void Actor::Impl::Init(Scene& scene, dynamics::World& world, int plane)
{
  world.Begin(function::Bind(&Impl::Begin, shared_from_this()));
  world.End(function::Bind(&Impl::End, shared_from_this()));
  scene.Add(function::Bind(&Impl::Render, shared_from_this()), plane);
  blink_.End(function::Bind(&Impl::Blink, shared_from_this()));
}

void Actor::Position(game::Position const& position)
{
  impl_->Position(position);
}

game::Position Actor::Position(void) const
{
  return impl_->Position();
}

void Actor::Velocity(game::Position const& velocity)
{
  impl_->Velocity(velocity);
}

game::Position Actor::Velocity(void) const
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

void Actor::Pause(void)
{
  impl_->Pause();
}

void Actor::Resume(void)
{
  impl_->Resume();
}

void Actor::Up(void)
{
  impl_->Up();
}

void Actor::Down(void)
{
  impl_->Down();
}

void Actor::Left(void)
{
  impl_->Left();
}

void Actor::Right(void)
{
  impl_->Right();
}

void Actor::Body(std::string const& expression)
{
  impl_->Body(expression);
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

Actor::operator bool(void) const
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