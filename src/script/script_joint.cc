#include "script_impl.h"
#include "bind.h"
namespace game
{
auto Script::Impl::JointInit() -> void
{
  lua_.Add(function::Bind(&Impl::JointLoad, shared_from_this()), "joint_load", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::JointFree, shared_from_this()), "joint_free", 0, "metallic_crow");
}

auto Script::Impl::JointLoad() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-5);
    stage = StageGet();
  }
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-4);
      lua_.Pop(name);
    }
    std::string actor_a_name;
    {
      lua::Guard guard = lua_.Get(-3);
      lua_.Pop(actor_a_name);
    }
    std::string actor_b_name;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(actor_b_name);
    }
    auto range_a = stage->actors_.equal_range(actor_a_name);
    auto range_b = stage->actors_.equal_range(actor_b_name);
    for(auto& actor_a = range_a.first; actor_a != range_a.second; ++actor_a)
    {
      for(auto& actor_b = range_b.first; actor_b != range_b.second; ++actor_b)
      {
        lua::Guard guard = lua_.Get(-1);
        stage->joints_.emplace(name, Joint(lua_, actor_a->second, actor_b->second, stage->world_));
      }
    }
  }
}

auto Script::Impl::JointFree() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-2);
    stage = StageGet();
  }
  if (stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(name);
    }
    stage->joints_.erase(name);
  }
}
}