#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::ViewInit()
{
  lua_.Add(function::Bind(&Impl::ViewAddActor, shared_from_this()), "view_add", 0);
  lua_.Add(function::Bind(&Impl::ViewActor, shared_from_this()), "view", 0);
  lua_.Add(function::Bind(&Impl::ViewZoom, shared_from_this()), "view_zoom", 0);
}

void Script::Impl::ViewAddActor()
{
  StagePtr stage = StagePop();
  std::string name;
  lua_.PopFront(name);
  if(stage)
  {
    auto range = stage->actors_.equal_range(name);
    for(auto& actor = range.first; actor != range.second; ++actor)
    {
      stage->subjects_.push_back(actor->second);
    }
  }
}

void Script::Impl::ViewActor()
{
  StagePtr stage = StagePop();
  std::string name;
  lua_.PopFront(name);
  if(stage)
  {
    stage->subjects_.clear();
    auto range = stage->actors_.equal_range(name);
    for(auto& actor = range.first; actor != range.second; ++actor)
    {
      stage->subjects_.push_back(actor->second);
    }
  }
}

void Script::Impl::ViewZoom()
{
  StagePtr stage = StagePop();
  float z;
  lua_.PopFront(z);
  if(stage)
  {
    stage->zoom_ = z;
  }
}

void Script::Impl::View()
{
  game::Position view(0.f, 0.f);
  int count = 0;

  for(auto iter = stage_.second->subjects_.begin(); iter != stage_.second->subjects_.end();)
  {
    if(Actor subject = iter->Lock())
    {
      game::Position position = subject.Position();
      view.first += position.first;
      view.second += position.second;
      ++count;
      ++iter;
    }
    else
    {
      iter = stage_.second->subjects_.erase(iter);
    }
  }

  if(count)
  {
    window_.View(view.first / count, view.second / count, stage_.second->zoom_);
  }
}
}