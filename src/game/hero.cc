#include "hero.h"
#include "bounding_box.h"
#include "event.h"
#include "dynamics.h"
#include "bind.h"
#include "state.h"
namespace game
{
class HeroImpl final : public std::enable_shared_from_this<HeroImpl>
{
public:
  HeroImpl(json::JSON const& json, display::Window& window, event::Queue& queue, dynamics::World& world);
  void Init(Scene& scene, RulesCollision& collision, DynamicsCollision& dcollision, CommandCollision& ccollision, dynamics::World& world);
  void End(event::Command const& command);
  State moving_;
  State destroyed_;
  State spawn_;
  State idle_;
  State hit_;
  State current_;
  display::BoundingBox render_box_;
  bool paused_;
  event::Signal end_;
  event::Signal life_signal_;
  dynamics::Body body_;
  int x_sign_;
  int x_facing_;
  int y_sign_;
  int y_facing_;
  int life_;
  game::Position force_;
  void Render(void) const;
  void Pause(void);
  void Resume(void);
  void Up(void);
  void Down(void);
  void Left(void);
  void Right(void);
  void Update(void);
  void Attack(void);
  RulesCollision::Rules EnemySend(void);
  void EnemyReceive(RulesCollision::Rules const& rules);
  void SignalEnd(void);
  void Reset(void);
  void Step(float dt);
  void Change(State& next);
  void Life(Hero::Command command);
  void Position(game::Position const& position);
  game::Position Position(void);
  void BodyUpdate(void);
  void ItemCollide(void);
};

void HeroImpl::End(event::Command const& command)
{
  end_.Add(command);
}

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

void HeroImpl::Attack(void)
{
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

RulesCollision::Rules HeroImpl::EnemySend(void)
{
  return RulesCollision::Rules();
}

void HeroImpl::EnemyReceive(RulesCollision::Rules const& rules)
{
  life_ -= rules.first;
  if(life_ <= 0)
  {
    life_ = 0;
    Change(destroyed_);
  }
  else
  {
    Change(hit_);
  }
  life_signal_();
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

void HeroImpl::SignalEnd(void)
{
  end_();
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
  life_ = 100;
  force_ = game::Position(0.f, 0.f);
  render_box_ = display::BoundingBox(0.f, 0.f, 0.f, 0.f);
  BodyUpdate();
}

void HeroImpl::Init(Scene& scene, RulesCollision& collision, DynamicsCollision& dcollision, CommandCollision& ccollision, dynamics::World& world)
{
  auto ptr = shared_from_this();
  world.Add(event::Bind(&HeroImpl::BodyUpdate, ptr));
  hit_.End(event::Bind(&HeroImpl::Reset, ptr));
  destroyed_.End(event::Bind(&HeroImpl::SignalEnd, ptr));
  spawn_.End(event::Bind(&HeroImpl::Reset, ptr));
  scene.Add(event::Bind(&HeroImpl::Render, ptr), 0);
  event::pause.first.Add(event::Bind(&HeroImpl::Pause, ptr));
  event::pause.second.Add(event::Bind(&HeroImpl::Resume, ptr));
  event::up.first.Add(event::Bind(&HeroImpl::Up, ptr));
  event::up.second.Add(event::Bind(&HeroImpl::Down, ptr));
  event::down.first.Add(event::Bind(&HeroImpl::Down, ptr));
  event::down.second.Add(event::Bind(&HeroImpl::Up, ptr));
  event::left.first.Add(event::Bind(&HeroImpl::Left, ptr));
  event::left.second.Add(event::Bind(&HeroImpl::Right, ptr));
  event::right.first.Add(event::Bind(&HeroImpl::Right, ptr));
  event::right.second.Add(event::Bind(&HeroImpl::Left, ptr));
  event::button1.first.Add(event::Bind(&HeroImpl::Attack, ptr));
  RulesCollision::Send send(event::Bind(&HeroImpl::EnemySend, ptr));
  RulesCollision::Receive receive(event::Bind(&HeroImpl::EnemyReceive, ptr));
  RulesCollision::Channel channel(send, receive);
  collision.Add(0, body_, channel);
  dcollision.Add(0, body_);
  ccollision.Add(0, body_, event::Bind(&HeroImpl::ItemCollide, ptr), true);
}

void HeroImpl::Life(Hero::Command command)
{
  life_signal_.Add([=](){return command(life_);});
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

void Hero::End(event::Command const& command)
{
  impl_->End(command);
}

void Hero::Life(Command const& command)
{
  impl_->Life(command);
}

Hero::Hero(json::JSON const& json, display::Window& window, Scene& scene, RulesCollision& collision, DynamicsCollision& dcollision, CommandCollision& ccollision, event::Queue& queue, dynamics::World& world)
{
  impl_ = std::make_shared<HeroImpl>(json, window, queue, world);
  impl_->Init(scene, collision, dcollision, ccollision, world);
}
}