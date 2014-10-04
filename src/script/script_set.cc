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
  std::string name;
  std::string file;
  lua_.PopFront(name);
  lua_.PopFront(file);
  stage_->sets_.emplace(name, Set(json::JSON(file), window_, stage_->scene_, stage_->group_, stage_->world_));
}

void Script::Impl::SetFree()
{
  std::string name;
  lua_.PopFront(name);
  stage_->sets_.erase(name);
}

void Script::Impl::SetLight()
{
  std::string name;
  float r, g, b;
  lua_.PopFront(name);
  lua_.PopFront(r);
  lua_.PopFront(g);
  lua_.PopFront(b);
  auto range = stage_->sets_.equal_range(name);
  for(auto& set = range.first; set != range.second; ++set)
  {
    set->second.Modulation(r, g, b);
  }
}
}