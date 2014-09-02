#include "hero.h"
#include "bounding_box.h"
#include "dynamics.h"
#include "bind.h"
#include "avatar.h"
#include "json_iterator.h"
namespace game
{
class HeroImpl final : public std::enable_shared_from_this<HeroImpl>
{
public:
  HeroImpl(json::JSON const& json, display::Window& window, event::Queue& queue, dynamics::World& world);
  void Init(Scene& scene, DynamicsCollision& dcollision, CommandCollision& ccollision, dynamics::World& world);
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
  game::Position Position(void);
  void Begin(void);
  void End(void);
  void Collide(void);

  Avatar avatar_;
  dynamics::Body body_;
  
  bool paused_;
  int x_sign_;
  int y_sign_;
  game::Position force_;
  float magnitude_;
};

void HeroImpl::Pause(void)
{
  paused_ = true;
  avatar_.Pause();
}

void HeroImpl::Resume(void)
{
  paused_ = false;
  avatar_.Resume();
}

void HeroImpl::Up(void)
{
  if(!paused_)
  {
    --y_sign_;
    Update();
  }
}

void HeroImpl::Down(void)
{
  if(!paused_)
  {
    ++y_sign_;
    Update();
  }
}

void HeroImpl::Left(void)
{
  if(!paused_)
  {
    --x_sign_;
    Update();
  }
}

void HeroImpl::Right(void)
{
  if(!paused_)
  {
    ++x_sign_;
    Update();
  }
}

void HeroImpl::Update(void)
{
  float force = magnitude_;
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

void HeroImpl::Begin(void)
{
  body_.Force(force_.first, force_.second);
}

void HeroImpl::End(void)
{
  avatar_.Position(body_.Position());
  avatar_.Modulation(body_.Modulation());
}

void HeroImpl::Render(void) const
{
  avatar_.Render();
}

void HeroImpl::Collide(void)
{
}

void HeroImpl::Position(game::Position const& position)
{
  body_.Position(position.first, position.second);
}

game::Position HeroImpl::Position(void)
{
  return body_.Position();
}

void HeroImpl::Body(std::string const& expression)
{
  avatar_.Body(expression);
}

void HeroImpl::Eyes(std::string const& expression)
{
  avatar_.Eyes(expression);
}

void HeroImpl::Mouth(std::string const& expression)
{
  avatar_.Mouth(expression);
}

void HeroImpl::Mouth(int open)
{
  avatar_.Mouth(open);
}

HeroImpl::HeroImpl(json::JSON const& json, display::Window& window, event::Queue& queue, dynamics::World& world) : 
  paused_(true), 
  x_sign_(0), 
  y_sign_(0), 
  force_(0.f, 0.f), 
  magnitude_(2000.f)
{
  json_t* avatar;
  json_t* body;
  json.Unpack("{soso}",
    "body", &body,
    "avatar", &avatar);
  body_ = dynamics::Body(json::JSON(body), world);
  avatar_ = Avatar(json::JSON(avatar), window, queue);
  avatar_.Body("idle");
  avatar_.Eyes("idle");
  avatar_.Mouth("idle");
}

void HeroImpl::Init(Scene& scene, DynamicsCollision& dcollision, CommandCollision& ccollision, dynamics::World& world)
{
  auto ptr = shared_from_this();
  world.Begin(event::Bind(&HeroImpl::Begin, ptr));
  world.End(event::Bind(&HeroImpl::End, ptr));
  scene.Add(event::Bind(&HeroImpl::Render, ptr), 0);
 
  dcollision.Add(dynamics::Type::Hero, body_);
  ccollision.Add(dynamics::Type::Hero, body_, event::Bind(&HeroImpl::Collide, ptr), true);
}

void Hero::Position(game::Position const& position)
{
  impl_->Position(position);
}

game::Position Hero::Position(void)
{
  return impl_->Position();
}

void Hero::Pause(void)
{
  impl_->Pause();
}

void Hero::Resume(void)
{
  impl_->Resume();
}

void Hero::Up(void)
{
  impl_->Up();
}

void Hero::Down(void)
{
  impl_->Down();
}

void Hero::Left(void)
{
  impl_->Left();
}

void Hero::Right(void)
{
  impl_->Right();
}

void Hero::Body(std::string const& expression)
{
  impl_->Body(expression);
}

void Hero::Eyes(std::string const& expression)
{
  impl_->Eyes(expression);
}

void Hero::Mouth(std::string const& expression)
{
  impl_->Mouth(expression);
}

void Hero::Mouth(int open)
{
  impl_->Mouth(open);
}

Hero::operator bool(void) const
{
  return bool(impl_);
}

Hero::Hero(json::JSON const& json, display::Window& window, Scene& scene, DynamicsCollision& dcollision, CommandCollision& ccollision, event::Queue& queue, dynamics::World& world) : impl_(std::make_shared<HeroImpl>(json, window, queue, world))
{
  impl_->Init(scene, dcollision, ccollision, world);
}
}