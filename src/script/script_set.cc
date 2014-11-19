#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::SetInit()
{
  lua_.Add(function::Bind(&Impl::SetLoad, shared_from_this()), "set_load", 0);
  lua_.Add(function::Bind(&Impl::SetFree, shared_from_this()), "set_free", 0);
  lua_.Add(function::Bind(&Impl::SetLight, shared_from_this()), "set_light", 0);
}

void Script::Impl::SetLoad()
{
  StagePtr stage = StagePop();
  std::string name;
  std::string file;
  lua_.PopFront(name);
  lua_.PopFront(file);
  if(stage)
  {
    stage->sets_.emplace(name, Set(json::JSON(path_ / file), window_, stage->scene_, stage->group_, stage->world_, path_));
  }
}

void Script::Impl::SetFree()
{
  StagePtr stage = StagePop();
  std::string name;
  lua_.PopFront(name);
  if(stage)
  {
    stage->sets_.erase(name);
  }
}

void Script::Impl::SetLight()
{
  StagePtr stage = StagePop();
  std::string name;
  float r, g, b;
  lua_.PopFront(name);
  lua_.PopFront(r);
  lua_.PopFront(g);
  lua_.PopFront(b);
  if(stage)
  {
    auto range = stage->sets_.equal_range(name);
    for(auto& set = range.first; set != range.second; ++set)
    {
      set->second.Modulation(r, g, b);
    }
  }
}
}