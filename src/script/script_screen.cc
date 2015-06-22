#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::ScreenInit()
{
  lua_.Add(function::Bind(&Impl::ScreenLoad, shared_from_this()), "screen_load", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ScreenFree, shared_from_this()), "screen_free", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ScreenModulation, shared_from_this()), "screen_modulation", 0, "metallic_crow");
}

void Script::Impl::ScreenLoad()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-3);
    stage = StageGet();
  }
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(name);
    }

    lua::Guard guard = lua_.Get(-1);
    stage->screens_.emplace(name, Screen(lua_, window_, stage->scene_, queue_, path_));
  }
}

void Script::Impl::ScreenFree()
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
    stage->screens_.erase(name);
  }
}

void Script::Impl::ScreenModulation()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-6);
    stage = StageGet();
  }
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-5);
      lua_.Pop(name);
    }
    float r;
    {
      lua::Guard guard = lua_.Get(-4);
      lua_.Pop(r);
    }
    float g;
    {
      lua::Guard guard = lua_.Get(-3);
      lua_.Pop(g);
    }
    float b;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(b);
    }
    float a;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(a);
    }

    auto range = stage->screens_.equal_range(name);
    for(auto& screen = range.first; screen != range.second; ++screen)
    {
      screen->second.Modulation(r, g, b, a);
    }
  }
}
}