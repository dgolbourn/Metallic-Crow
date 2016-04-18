#include "script_impl.h"
#include "bind.h"
#include <limits>
namespace game
{
auto Script::Impl::ViewInit() -> void
{
  lua_.Add(function::Bind(&Impl::ViewAddActor, shared_from_this()), "view_add", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ViewActor, shared_from_this()), "view", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ViewZoom, shared_from_this()), "view_zoom", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ViewRotation, shared_from_this()), "view_rotation", 0, "metallic_crow");
}

auto Script::Impl::ViewAddActor() -> void
{
  std::pair<StagePtr, Actor> actor = StageDataGet<Actor>();
  if(actor.first && actor.second)
  {
    actor.first->subjects_.emplace(actor.second);
  }
}

auto Script::Impl::ViewActor() -> void
{
  std::pair<StagePtr, Actor> actor = StageDataGet<Actor>();
  if(actor.first && actor.second)
  {
    actor.first->subjects_.clear();
    actor.first->subjects_.emplace(actor.second);
  }
}

auto Script::Impl::ViewZoom() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-2);
    stage = StageGet();
  }
  if(stage)
  {
    stage->zoom_ = lua_.At<float>(-1);
  }
}

auto Script::Impl::ViewRotation() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-2);
    stage = StageGet();
  }
  if(stage)
  {
    stage->angle_ = lua_.At<double>(-1);
    if(stage_ = stage)
    {
      window_.Rotation(stage->angle_);
    }
  }
}

auto Script::Impl::View() -> void
{
  game::Position view(0.f, 0.f);
  game::Position min(std::numeric_limits<float>().max(), std::numeric_limits<float>().max());
  game::Position max(-std::numeric_limits<float>().max(), -std::numeric_limits<float>().max());

  for(Actor actor : stage_->subjects_)
  {    
    game::Position position = actor.Position();
    view.first += position.first;
    view.second += position.second;

    min.first = std::min(min.first, position.first);
    min.second = std::min(min.second, position.second);
    max.first = std::max(max.first, position.first);
    max.second = std::max(max.second, position.second);
  }

  if(auto count = stage_->subjects_.size())
  {
    window_.View(view.first / count, view.second / count, std::min(stage_->zoom_, Scale() * window_.Scale() / std::max(max.first - min.first, max.second - min.second)));
  }
}

auto Script::Impl::Scale() const -> float
{
  return .75f;
}
}