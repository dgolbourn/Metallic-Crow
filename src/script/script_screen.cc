#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::ScreenInit()
{
  lua_.Add(function::Bind(&Impl::ScreenLoad, shared_from_this()), "screen_load", 0);
  lua_.Add(function::Bind(&Impl::ScreenFree, shared_from_this()), "screen_free", 0);
  lua_.Add(function::Bind(&Impl::ScreenModulation, shared_from_this()), "screen_modulation", 0);
}

void Script::Impl::ScreenLoad()
{
  StagePtr stage = StagePop();
  std::string name;
  std::string file;
  lua_.PopFront(name);
  lua_.PopFront(file);
  if(stage)
  {
    stage->screens_.emplace(name, Screen(json::JSON(path_ / file), window_, stage->scene_, queue_, path_));
  }
}

void Script::Impl::ScreenFree()
{
  StagePtr stage = StagePop();
  std::string name;
  lua_.PopFront(name);
  if(stage)
  {
    stage->screens_.erase(name);
  }
}

void Script::Impl::ScreenModulation()
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
    auto range = stage->screens_.equal_range(name);
    for(auto& screen = range.first; screen != range.second; ++screen)
    {
      screen->second.Modulation(r, g, b, a);
    }
  }
}
}