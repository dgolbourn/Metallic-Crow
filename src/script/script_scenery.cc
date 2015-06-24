#include "script_impl.h"
#include "bind.h"
namespace game
{
auto Script::Impl::SceneryInit() -> void
{
  lua_.Add(function::Bind(&Impl::SceneryLoad, shared_from_this()), "scenery_load", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::SceneryFree, shared_from_this()), "scenery_free", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::SceneryModulation, shared_from_this()), "scenery_modulation", 0, "metallic_crow");
}

auto Script::Impl::SceneryLoad() -> void
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
    stage->scenery_.emplace(name, Scenery(lua_, queue_, window_, stage->scene_, path_));
  }
}

auto Script::Impl::SceneryFree() -> void
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
    stage->scenery_.erase(name);
  }
}

auto Script::Impl::SceneryModulation() -> void
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

    auto range = stage->scenery_.equal_range(name);
    for(auto& scenery = range.first; scenery != range.second; ++scenery)
    {
      scenery->second.Modulation(r, g, b, a);
    }
  }
}
}