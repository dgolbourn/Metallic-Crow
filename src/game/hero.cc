#include "hero.h"
#include "bounding_box.h"
#include "event.h"
#include "dynamics.h"
#include "bind.h"
#include "state.h"

namespace game
{
class HeroImpl : public std::enable_shared_from_this<HeroImpl>
{
public:
  HeroImpl(json::JSON const& json, display::Window& window, event::Queue& queue);
  void Init(Scene& scene, RulesCollision& collision);
  void End(event::Command const& command);
  std::mutex mutex_;
  State moving_;
  State destroyed_;
  State spawn_;
  State idle_;
  State hit_;
  State current_;
  display::BoundingBox render_box_;
  display::BoundingBox collision_box_;
  bool paused_;
  event::Signal end_;
  event::Signal life_signal_;
  game::Position position_;
  Dynamics dynamics_;
  int x_sign_;
  int x_facing_;
  int y_sign_;
  int y_facing_;
  int life_;
  void Render(void);
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

static float const dv = 250.f;
static float const sqrt1_2 = std::sqrt(0.5f);

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

void HeroImpl::Update(void)
{
  float v = dv;
  if(x_sign_ && y_sign_)
  {
    v *= sqrt1_2;
  }
  if(x_sign_)
  {
    x_facing_ = x_sign_;
  }

  dynamics_.u(float(x_sign_) * v);
  dynamics_.v(float(y_sign_) * v);

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

void HeroImpl::Change(State& next)
{
  current_.Stop();
  current_ = next;
  current_.Play();
  if(paused_)
  {
    current_.Pause();
  }
}

static void BoxUpdate(display::BoundingBox const& source, display::BoundingBox& destination, Position const& position, int x_facing)
{
  display::BoundingBox temp = source.Copy();
  int sourcex = temp.x();
  temp.x(temp.x() + position.first);
  temp.y(temp.y() + position.second);
  if(x_facing < 0)
  {
    temp.x(temp.x() - 2 * sourcex);
    temp.w(-temp.w());
  }
  destination.Copy(temp);
}

void HeroImpl::Step(float dt)
{
  dynamics_.Step(dt);
  position_.first = int(dynamics_.x()); 
  position_.second = int(dynamics_.y());
  BoxUpdate(current_.Render(), render_box_, position_, x_facing_);
  BoxUpdate(current_.Collision(), collision_box_, position_, x_facing_);
}

void HeroImpl::Render(void)
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

HeroImpl::HeroImpl(json::JSON const& json, display::Window& window, event::Queue& queue)
{
  json_t* moving;
  json_t* destroyed;
  json_t* spawn;
  json_t* idle;
  json_t* hit;

  json.Unpack("{sososososo}",
    "moving", &moving,
    "destroyed", &destroyed,
    "spawn", &spawn,
    "idle", &idle,
    "hit", &hit);
  
  paused_ = true;
  moving_ = State(moving, window, queue);
  destroyed_ = State(destroyed, window, queue);
  idle_ = State(idle, window, queue);
  hit_ = State(hit, window, queue);
  spawn_ = State(spawn, window, queue);
  current_ = spawn_;
  current_.Play();
  current_.Pause();
  collision_box_ = current_.Collision().Copy();
  render_box_ = current_.Render().Copy();
  position_ = game::Position(0, 0);
  dynamics_ = Dynamics(0.f, 0.f, 0.f, 0.f);
  x_sign_ = 0;
  y_sign_ = 0;
  x_facing_ = 0;
  y_facing_ = 0;
  life_ = 100;
}

void HeroImpl::Init(Scene& scene, RulesCollision& collision)
{
  auto ptr = shared_from_this();
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
  collision.Add(0, ptr->collision_box_, channel);
}

void HeroImpl::Life(Hero::Command command)
{
  life_signal_.Add([=](){return command(life_);});
}

void Hero::Position(game::Position const& position)
{
  thread::Lock lock(impl_->mutex_);
  impl_->position_ = position;
  impl_->dynamics_.x(float(position.first));
  impl_->dynamics_.y(float(position.second));
}

game::Position Hero::Position(void)
{
  thread::Lock lock(impl_->mutex_);
  return impl_->position_;
}

void Hero::End(event::Command const& command)
{
  thread::Lock lock(impl_->mutex_);
  impl_->End(command);
}

void Hero::Life(Command const& command)
{
  thread::Lock lock(impl_->mutex_);
  impl_->Life(command);
}

void Hero::Step(float dt)
{
  thread::Lock lock(impl_->mutex_);
  impl_->Step(dt);
}

Hero::Hero(json::JSON const& json, display::Window& window, Scene& scene, RulesCollision& collision, event::Queue& queue)
{
  impl_ = std::make_shared<HeroImpl>(json, window, queue);
  thread::Lock lock(impl_->mutex_);
  impl_->Init(scene, collision);
}
}