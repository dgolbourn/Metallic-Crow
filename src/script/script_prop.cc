#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::PropInit()
{
  lua_.Add(function::Bind(&Impl::PropLoad, shared_from_this()), "prop_load", 0);
  lua_.Add(function::Bind(&Impl::PropFree, shared_from_this()), "prop_free", 0);
  lua_.Add(function::Bind(&Impl::PropLight, shared_from_this()), "prop_light", 0);
}

void Script::Impl::PropLoad()
{
  std::string name;
  std::string file;
  lua_.PopFront(name);
  lua_.PopFront(file);
  stage_->prop_.emplace(name, Prop(json::JSON(file), window_, stage_->scene_, stage_->group_, stage_->world_));
}

void Script::Impl::PropFree()
{
  std::string name;
  lua_.PopFront(name);
  stage_->prop_.erase(name);
}

void Script::Impl::PropLight()
{
  std::string name;
  float r, g, b;
  lua_.PopFront(name);
  lua_.PopFront(r);
  lua_.PopFront(g);
  lua_.PopFront(b);
  auto range = stage_->prop_.equal_range(name);
  for(auto& prop = range.first; prop != range.second; ++prop)
  {
    prop->second.Modulation(r, g, b);
  }
}
}