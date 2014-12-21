#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::SceneryInit()
{
  lua_.Add(function::Bind(&Impl::SceneryLoad, shared_from_this()), "scenery_load", 0);
  lua_.Add(function::Bind(&Impl::SceneryFree, shared_from_this()), "scenery_free", 0);
  lua_.Add(function::Bind(&Impl::SceneryModulation, shared_from_this()), "scenery_modulation", 0);
}

void Script::Impl::SceneryLoad()
{
  StagePtr stage = StagePop();
  std::string name;
  std::string file;
  lua_.PopFront(name);
  lua_.PopFront(file);
  if(stage)
  {
    stage->scenery_.emplace(name, Scenery(json::JSON(path_ / file), queue_, window_, stage->scene_, path_));
  }
}

void Script::Impl::SceneryFree()
{
  StagePtr stage = StagePop();
  std::string name;
  lua_.PopFront(name);
  if(stage)
  {
    stage->scenery_.erase(name);
  }
}

void Script::Impl::SceneryModulation()
{
  StagePtr stage = StagePop();
  std::string name;
  float r, g, b, a;
  lua_.PopFront(name);
  lua_.PopFront(r);
  lua_.PopFront(g);
  lua_.PopFront(b);
  lua_.PopFront(a);
  if(stage)
  {
    auto range = stage->scenery_.equal_range(name);
    for(auto& scenery = range.first; scenery != range.second; ++scenery)
    {
      scenery->second.Modulation(r, g, b, a);
    }
  }
}
}