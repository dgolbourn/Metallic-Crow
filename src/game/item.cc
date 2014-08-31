#include "item.h"
#include "bounding_box.h"
#include "event.h"
#include "body.h"
#include "bind.h"
#include "state.h"
#include <map>
#include "json_iterator.h"
namespace game
{
typedef std::map<std::string, State> StateMap;

class ItemImpl final : public std::enable_shared_from_this<ItemImpl>
{
public:
  ItemImpl(void);
  void Init(json::JSON const& json, display::Window& window, event::Queue& queue, dynamics::World& world, CommandCollision& ccollision, Scene& scene);
  void Add(event::Command const& start, event::Command const& end);
  void Proximity(event::Command const& start, event::Command const& end);
  void Hysteresis(event::Command const& start, event::Command const& end);
  void Pause(void);
  void Resume(void);
  void Position(game::Position const& position);
  game::Position Position(void) const;
  void Update(void);
  void Render(void) const;
  void Change(std::string const& state);
  void ProximityCollideStart(void);
  void ProximityCollideEnd(void);
  void InteractionCollideStart(void);
  void InteractionCollideEnd(void);
  StateMap states_;
  State state_;
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
  state_.Pause();
}

void ItemImpl::Resume(void)
{
  paused_ = false;
  state_.Resume();
}

void ItemImpl::Update(void)
{
  game::Position position = Position();
  display::BoundingBox temp = state_.Shape().Copy();
  temp.x(temp.x() + position.first);
  temp.y(temp.y() + position.second);
  render_box_.Copy(temp);
}

void ItemImpl::Render(void) const
{
  state_.Render(render_box_, 1.f, false, 0., interaction_.Modulation());
}

ItemImpl::ItemImpl(void) : paused_(true), hysteresis_(false), render_box_(display::BoundingBox(0.f, 0.f, 0.f, 0.f))
{
}

void ItemImpl::Init(json::JSON const& json, display::Window& window, event::Queue& queue, dynamics::World& world, CommandCollision& ccollision, Scene& scene)
{
  auto ptr = shared_from_this();
  scene.Add(event::Bind(&ItemImpl::Render, ptr), -1);

  json_t* interaction;
  json_t* proximity;
  json_t* states;

  json.Unpack("{sososo}",
    "interaction", &interaction,
    "proximity", &proximity,
    "states", &states);
  interaction_ = dynamics::Body(json::JSON(interaction), world);
  proximity_ = dynamics::Body(json::JSON(proximity), world);

  ccollision.Add(dynamics::Type::Proximity, proximity_, event::Bind(&ItemImpl::ProximityCollideStart, ptr), true);
  ccollision.Add(dynamics::Type::Proximity, proximity_, event::Bind(&ItemImpl::ProximityCollideEnd, ptr), false);
  ccollision.Add(dynamics::Type::Interaction, interaction_, event::Bind(&ItemImpl::InteractionCollideStart, ptr), true);
  ccollision.Add(dynamics::Type::Interaction, interaction_, event::Bind(&ItemImpl::InteractionCollideEnd, ptr), false);

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
        valid.first->second.End(event::Bind(&ItemImpl::Change, ptr, next_str));
      }
    }
  }

  Change("idle");
}

void ItemImpl::ProximityCollideStart(void)
{
  Change("active");
  proximity_event_.first();
}

void ItemImpl::ProximityCollideEnd(void)
{
  Change("idle");
  proximity_event_.second();
  if(hysteresis_)
  {
    hysteresis_ = false;
    hysteresis_event_.second();
  }
}

void ItemImpl::InteractionCollideStart(void)
{
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

void ItemImpl::Change(std::string const& next)
{
  auto iter = states_.find(next);
  if(iter != states_.end())
  {
    if(!(state_ == iter->second))
    {
      if(state_)
      {
        state_.Reset();
      }
      state_ = iter->second;
      if(!paused_)
      {
        state_.Resume();
      }
    }
  }
  Update();
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

void Item::State(std::string const& state)
{
  impl_->Change(state);
}

Item::Item(json::JSON const& json, display::Window& window, Scene& scene, event::Queue& queue, CommandCollision& ccollision, dynamics::World& world)
{
  impl_ = std::make_shared<ItemImpl>();
  impl_->Init(json, window, queue, world, ccollision, scene);
}
}