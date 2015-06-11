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
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-2);
    stage = StageGet();
  }
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(name);
    }
    auto range = stage->actors_.equal_range(name);
    for(auto& actor = range.first; actor != range.second; ++actor)
    {
      stage->subjects_.push_back(actor->second);
    }
  }
}

void Script::Impl::ViewActor()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-2);
    stage = StageGet();
  }  
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(name);
    }

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
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-2);
    stage = StageGet();
  }
  if(stage)
  {
    lua::Guard guard = lua_.Get(-1);
    lua_.Pop(stage->zoom_);
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