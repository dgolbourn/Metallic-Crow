#include "hero.h"
#include "bounding_box.h"
#include "dynamics.h"
#include "bind.h"
#include "state.h"
#include <map>
#include "json_iterator.h"
namespace game
{
typedef std::map<std::string, State> StateMap;

class HeroImpl final : public std::enable_shared_from_this<HeroImpl>
{
public:
  HeroImpl(void);
  void Init(json::JSON const& json, display::Window& window, event::Queue& queue, Scene& scene, DynamicsCollision& dcollision, CommandCollision& ccollision, dynamics::World& world);
  State state_;
  StateMap states_;
  display::BoundingBox render_box_;
  bool paused_;
  dynamics::Body body_;
  int x_sign_;
  int facing_;
  int y_sign_;
  game::Position force_;
  void Render(void) const;
  void Pause(void);
  void Resume(void);
  void Up(void);
  void Down(void);
  void Left(void);
  void Right(void);
  void Update(void);
  void Change(std::string const& next);
  void Position(game::Position const& position);
  game::Position Position(void);
  void Begin(void);
  void End(void);
  void Collide(void);
};

void HeroImpl::Pause(void)
{
  paused_ = true;
  state_.Pause();
}

void HeroImpl::Resume(void)
{
  paused_ = false;
  state_.Resume();
}

void HeroImpl::Up(void)
{
  --y_sign_;
  Update();
}

void HeroImpl::Down(void)
{
  ++y_sign_; 
  Update();
}

void HeroImpl::Left(void)
{
  --x_sign_;
  Update();
}

void HeroImpl::Right(void)
{
  ++x_sign_;
  Update();
}

static float const df = 2000.f;
static float const sqrt1_2 = std::sqrt(0.5f);

void HeroImpl::Update(void)
{
  if(!paused_)
  {
    float f = df;
    if(x_sign_ && y_sign_)
    {
      f *= sqrt1_2;
    }
    if(x_sign_)
    {
      facing_ = x_sign_;
    }

    force_ = game::Position(float(x_sign_) * f, float(y_sign_) * f);

    if(x_sign_ || y_sign_)
    {
      auto iter = states_.find("idle");
      if(iter != states_.end())
      {
        if(state_ == iter->second)
        {
          Change("active");
        }
      }
    }
    else
    {
      auto iter = states_.find("active");
      if(iter != states_.end())
      {
        if(state_ == iter->second)
        {
          Change("idle");
        }
      }
    }
  }
}

void HeroImpl::Change(std::string const& next)
{
  auto iter = states_.find(next);
  if(iter != states_.end())
  {
    if(!(state_ == iter->second))
    {
      if(state_)
      {
        state_.Stop();
      }
      state_ = iter->second;
      state_.Play();
      if(paused_)
      {
        state_.Pause();
      }
    }
  }
}

void HeroImpl::Begin(void)
{
  body_.Force(force_.first, force_.second);
}

void HeroImpl::End(void)
{
  display::BoundingBox temp = state_.Shape().Copy();
  game::Position position = body_.Position();
  float sourcex = temp.x();
  temp.x(sourcex + position.first);
  temp.y(temp.y() + position.second);
  if(facing_ < 0)
  {
    temp.x(temp.x() - 2.f * sourcex);
    temp.w(-temp.w());
  }
  render_box_.Copy(temp);
}

void HeroImpl::Render(void) const
{
  state_.Render(render_box_, 1.f, false, 0., body_.Modulation());
}

HeroImpl::HeroImpl(void) : paused_(true), x_sign_(0), y_sign_(0), facing_(0), force_(game::Position(0.f, 0.f)), render_box_(display::BoundingBox(0.f, 0.f, 0.f, 0.f))
{
}

void HeroImpl::Init(json::JSON const& json, display::Window& window, event::Queue& queue, Scene& scene, DynamicsCollision& dcollision, CommandCollision& ccollision, dynamics::World& world)
{
  auto ptr = shared_from_this();
  world.Begin(event::Bind(&HeroImpl::Begin, ptr));
  world.End(event::Bind(&HeroImpl::End, ptr));
  scene.Add(event::Bind(&HeroImpl::Render, ptr), 0);
  
  json_t* states;
  json_t* body;
  json.Unpack("{soso}",
    "body", &body,
    "states", &states);
  body_ = dynamics::Body(json::JSON(body), world);
  dcollision.Add(dynamics::Type::Hero, body_);
  ccollision.Add(dynamics::Type::Hero, body_, event::Bind(&HeroImpl::Collide, ptr), true);

  for(json::JSON const& value : json::JSON(states))
  {
    char const* name;
    json_t* state;
    char const* next;
    value.Unpack("{sssoss}",
      "name", &name,
      "state", &state,
      "next", &next);
    auto valid = states_.emplace(name, State(json::JSON(state), window, queue));
    if(valid.second)
    {
      std::string next_str(next);
      if(next_str != "")
      {
        valid.first->second.End(event::Bind(&HeroImpl::Change, ptr, next_str));
      }
    }
  }

  Change("spawn");
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

void Hero::State(std::string const& state)
{
  impl_->Change(state);
}

Hero::operator bool(void) const
{
  return bool(impl_);
}

Hero::Hero(json::JSON const& json, display::Window& window, Scene& scene, DynamicsCollision& dcollision, CommandCollision& ccollision, event::Queue& queue, dynamics::World& world)
{
  impl_ = std::make_shared<HeroImpl>();
  impl_->Init(json, window, queue, scene, dcollision, ccollision, world);
}
}