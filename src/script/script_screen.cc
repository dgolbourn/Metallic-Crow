#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::ScreenInit()
{
  lua_.Add(function::Bind(&Impl::ScreenLoad, shared_from_this()), "screen_load", 0);
  lua_.Add(function::Bind(&Impl::ScreenFree, shared_from_this()), "screen_free", 0);
  lua_.Add(function::Bind(&Impl::ScreenLight, shared_from_this()), "screen_light", 0);
}

void Script::Impl::ScreenLoad()
{
  std::string name;
  std::string file;
  lua_.PopFront(name);
  lua_.PopFront(file);
  stage_->screens_.emplace(name, Screen(json::JSON(file), window_, stage_->scene_, queue_));
}

void Script::Impl::ScreenFree()
{
  std::string name;
  lua_.PopFront(name);
  stage_->screens_.erase(name);
}

void Script::Impl::ScreenLight()
{
  std::string name;
  float r, g, b;
  lua_.PopFront(name);
  lua_.PopFront(r);
  lua_.PopFront(g);
  lua_.PopFront(b);
  auto range = stage_->screens_.equal_range(name);
  for(auto& screen = range.first; screen != range.second; ++screen)
  {
    screen->second.Modulation(r, g, b);
  }
}
}