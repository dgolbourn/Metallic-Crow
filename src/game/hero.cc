#include "hero.h"
#include "bounding_box.h"
#include "dynamics.h"
#include "bind.h"
#include "state.h"
namespace game
{
class HeroImpl final : public std::enable_shared_from_this<HeroImpl>
{
public:
  HeroImpl(json::JSON const& json, display::Window& window, event::Queue& queue, dynamics::World& world);
  void Init(Scene& scene, DynamicsCollision& dcollision, CommandCollision& ccollision, dynamics::World& world, event::Event& event);
  State moving_;
  State destroyed_;
  State spawn_;
  State idle_;
  State hit_;
  State current_;
  display::BoundingBox render_box_;
  bool paused_;
  dynamics::Body body_;
  int x_sign_;
  int x_facing_;
  int y_sign_;
  int y_facing_;
  game::Position force_;
  void Render(void) const;
  void Pause(void);
  void Resume(void);
  void Up(void);
  void Down(void);
  void Left(void);
  void Right(void);
  void Update(void);
  void Reset(void);
  void Change(State& next);
  void Position(game::Position const& position);
  game::Position Position(void);
  void BodyUpdate(void);
  void ItemCollide(void);
};

void HeroImpl::Pause(void)
{
  paused_ = true;
  current_.Pause();
}

void HeroImpl::Resume(void)
{
  paused_ = false;
  current_.Resume();
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
  float f = df;
  if(x_sign_ && y_sign_)
  {
    f *= sqrt1_2;
  }
  if(x_sign_)
  {
    x_facing_ = x_sign_;
  }

  force_ = game::Position(float(x_sign_) * f, float(y_sign_) * f);

  if(x_sign_ || y_sign_)
  {
    if(current_ == idle_)
    {
      Change(moving_);
    }
  }
  else
  {
    if(current_ == moving_)
    {
      Change(idle_);
    }
  }
}

void HeroImpl::ItemCollide(void)
{
}

void HeroImpl::Change(State& next)
{
  if(!(current_ == next))
  {
    current_.Stop();
    current_ = next;
    current_.Play();
    if(paused_)
    {
      current_.Pause();
    }
  }
}

static void BoxUpdate(display::BoundingBox const& source, display::BoundingBox& destination, Position const& position, int x_facing)
{
  display::BoundingBox temp = source.Copy();
  float sourcex = temp.x();
  temp.x(sourcex + position.first);
  temp.y(temp.y() + position.second);
  if(x_facing < 0)
  {
    temp.x(temp.x() - 2.f * sourcex);
    temp.w(-temp.w());
  }
  destination.Copy(temp);
}

void HeroImpl::BodyUpdate(void)
{
  BoxUpdate(current_.Shape(), render_box_, Position(), x_facing_);
  body_.Force(force_.first, force_.second);
}

void HeroImpl::Render(void) const
{
  current_.Render(render_box_);
}

void HeroImpl::Reset(void)
{
  Change(idle_); 
  Update();
}

HeroImpl::HeroImpl(json::JSON const& json, display::Window& window, event::Queue& queue, dynamics::World& world)
{
  json_t* moving;
  json_t* destroyed;
  json_t* spawn;
  json_t* idle;
  json_t* hit;
  json_t* body;

  json.Unpack("{sosososososo}",
    "moving", &moving,
    "destroyed", &destroyed,
    "spawn", &spawn,
    "idle", &idle,
    "hit", &hit,
    "body", &body);
  
  paused_ = true;
  moving_ = State(moving, window, queue);
  destroyed_ = State(destroyed, window, queue);
  idle_ = State(idle, window, queue);
  hit_ = State(hit, window, queue);
  spawn_ = State(spawn, window, queue);
  current_ = spawn_;
  current_.Play();
  current_.Pause();
  body_ = dynamics::Body(body, world);
  x_sign_ = 0;
  y_sign_ = 0;
  x_facing_ = 0;
  y_facing_ = 0;
  force_ = game::Position(0.f, 0.f);
  render_box_ = display::BoundingBox(0.f, 0.f, 0.f, 0.f);
  BodyUpdate();
}

void HeroImpl::Init(Scene& scene, DynamicsCollision& dcollision, CommandCollision& ccollision, dynamics::World& world, event::Event& event)
{
  auto ptr = shared_from_this();
  world.Add(event::Bind(&HeroImpl::BodyUpdate, ptr));
  hit_.End(event::Bind(&HeroImpl::Reset, ptr));
  spawn_.End(event::Bind(&HeroImpl::Reset, ptr));
  scene.Add(event::Bind(&HeroImpl::Render, ptr), 0);
  event.Up(event::Bind(&HeroImpl::Up, ptr), event::Bind(&HeroImpl::Down, ptr));
  event.Down(event::Bind(&HeroImpl::Down, ptr), event::Bind(&HeroImpl::Up, ptr));
  event.Left(event::Bind(&HeroImpl::Left, ptr), event::Bind(&HeroImpl::Right, ptr));
  event.Right(event::Bind(&HeroImpl::Right, ptr), event::Bind(&HeroImpl::Left, ptr));
  dcollision.Add(dynamics::Type::Hero, body_);
  ccollision.Add(dynamics::Type::Hero, body_, event::Bind(&HeroImpl::ItemCollide, ptr), true);
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

Hero::Hero(json::JSON const& json, display::Window& window, Scene& scene, DynamicsCollision& dcollision, CommandCollision& ccollision, event::Queue& queue, dynamics::World& world, event::Event& event)
{
  impl_ = std::make_shared<HeroImpl>(json, window, queue, world);
  impl_->Init(scene, dcollision, ccollision, world, event);
}
}