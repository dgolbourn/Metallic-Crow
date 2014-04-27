#include "item.h"
#include "bounding_box.h"
#include "event.h"
#include "body.h"
#include "bind.h"
#include "state.h"
#include "event.h"
namespace game
{
class ItemImpl final : public std::enable_shared_from_this<ItemImpl>
{
public:
  ItemImpl(json::JSON const& json, display::Window& window, event::Queue& queue, DynamicsCollision& dcollision, dynamics::World& world);
  void Init(CommandCollision& ccollision, Scene& scene);
  void Add(event::Command const& start, event::Command const& end);
  void Proximity(event::Command const& start, event::Command const& end);
  void Hysteresis(event::Command const& start, event::Command const& end);
  void Pause(void);
  void Resume(void);
  void Position(game::Position const& position);
  game::Position Position(void) const;
  void Update(void);
  void Render(void) const;
  void Change(State& next);
  void ProximityCollideStart(void);
  void ProximityCollideEnd(void);
  void Idle(void);
  void InteractionCollideStart(void);
  void InteractionCollideEnd(void);
  State idle_;
  State active_;
  State current_;
  display::BoundingBox render_box_;
  bool paused_;
  bool hysteresis_;
  dynamics::Body interaction_;
  dynamics::Body proximity_;
  event::Switch interaction_event_;
  event::Switch proximity_event_;
  event::Switch hysteresis_event_;
};

void ItemImpl::Pause(void)
{
  paused_ = true;
  current_.Pause();
}

void ItemImpl::Resume(void)
{
  paused_ = false;
  current_.Resume();
}

void ItemImpl::Update(void)
{
  game::Position position = Position();
  display::BoundingBox temp = current_.Shape().Copy();
  temp.x(temp.x() + position.first);
  temp.y(temp.y() + position.second);
  render_box_.Copy(temp);
}

void ItemImpl::Render(void) const
{
  current_.Render(render_box_);
}

ItemImpl::ItemImpl(json::JSON const& json, display::Window& window, event::Queue& queue, DynamicsCollision& dcollision, dynamics::World& world)
{
  json_t* idle;
  json_t* active;
  json_t* interaction;
  json_t* proximity;

  json.Unpack("{sosososo}",
    "idle", &idle,
    "active", &active,
    "interaction", &interaction,
    "proximity", &proximity);
  
  paused_ = true;
  hysteresis_ = false;
  idle_ = State(idle, window, queue);
  active_ = State(active, window, queue);
  current_ = idle_;
  current_.Play();
  current_.Pause();
  interaction_ = dynamics::Body(interaction, world);
  dcollision.Add(2, interaction_);
  proximity_ = dynamics::Body(proximity, world);
  dcollision.Add(3, proximity_);
  render_box_ = display::BoundingBox(0.f, 0.f, 0.f, 0.f);
  Update();
}

void ItemImpl::Init(CommandCollision& ccollision, Scene& scene)
{
  auto ptr = shared_from_this();
  scene.Add(event::Bind(&ItemImpl::Render, ptr), -1);
  ccollision.Add(3, proximity_, event::Bind(&ItemImpl::ProximityCollideStart, ptr), true);
  ccollision.Add(3, proximity_, event::Bind(&ItemImpl::ProximityCollideEnd, ptr), false);
  ccollision.Add(4, interaction_, event::Bind(&ItemImpl::InteractionCollideStart, ptr), true);
  ccollision.Add(4, interaction_, event::Bind(&ItemImpl::InteractionCollideEnd, ptr), false);
  event::pause.first.Add(event::Bind(&ItemImpl::Pause, ptr));
  event::pause.second.Add(event::Bind(&ItemImpl::Resume, ptr));
  active_.End(event::Bind(&ItemImpl::Idle, ptr));
}

void ItemImpl::ProximityCollideStart(void)
{
  Change(active_);
  proximity_event_.first();
}

void ItemImpl::ProximityCollideEnd(void)
{
  proximity_event_.second();
  if(hysteresis_)
  {
    hysteresis_ = false;
    hysteresis_event_.second();
  }
}

void ItemImpl::InteractionCollideStart(void)
{
  Change(active_);
  interaction_event_.first();
  if(!hysteresis_)
  {
    hysteresis_ = true;
    hysteresis_event_.first();
  }
}

void ItemImpl::InteractionCollideEnd(void)
{
  interaction_event_.second();
}

void ItemImpl::Idle(void)
{
  Change(idle_);
}

void ItemImpl::Change(State& next)
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
    Update();
  }
}

void ItemImpl::Position(game::Position const& position)
{
  interaction_.Position(position.first, position.second);
  proximity_.Position(position.first, position.second);
  Update();
}

game::Position ItemImpl::Position(void) const
{
  return interaction_.Position();
}

void ItemImpl::Add(event::Command const& start, event::Command const& end)
{
  interaction_event_.first.Add(start);
  interaction_event_.second.Add(end);
}

void ItemImpl::Proximity(event::Command const& start, event::Command const& end)
{
  proximity_event_.first.Add(start);
  proximity_event_.second.Add(end);
}

void ItemImpl::Hysteresis(event::Command const& start, event::Command const& end)
{
  hysteresis_event_.first.Add(start);
  hysteresis_event_.second.Add(end);
}

void Item::Add(event::Command const& start, event::Command const& end)
{
  impl_->Add(start, end);
}

void Item::Proximity(event::Command const& start, event::Command const& end)
{
  impl_->Proximity(start, end);
}

void Item::Hysteresis(event::Command const& start, event::Command const& end)
{
  impl_->Hysteresis(start, end);
}

void Item::Position(game::Position const& position)
{
  impl_->Position(position);
}

game::Position Item::Position(void) const
{
  return impl_->Position();
}

void Item::Pause(void)
{
  impl_->Pause();
}

void Item::Resume(void)
{
  impl_->Resume();
}

Item::Item(json::JSON const& json, display::Window& window, Scene& scene, event::Queue& queue, DynamicsCollision& dcollision, CommandCollision& ccollision, dynamics::World& world)
{
  impl_ = std::make_shared<ItemImpl>(json, window, queue, dcollision, world);
  impl_->Init(ccollision, scene);
}
}