#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::CollisionInit()
{
  lua_.Add(function::Bind(&Impl::CollisionBegin, shared_from_this()), "collision_begin", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::CollisionEnd, shared_from_this()), "collision_end", 0, "metallic_crow");
}

void Script::Impl::CollisionBegin()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-4);
    stage = StageGet();
  }
  if(stage)
  {
    std::string group_a;
    {
      lua::Guard guard = lua_.Get(-3);
      lua_.Pop(group_a);
    }

    std::string group_b;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(group_b);
    }

    event::Command command;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(command);
    }    
    
    stage->group_.Begin(group_a, group_b, command);
  }
}

void Script::Impl::CollisionEnd()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-4);
    stage = StageGet();
  }
  if(stage)
  {
    std::string group_a;
    {
      lua::Guard guard = lua_.Get(-3);
      lua_.Pop(group_a);
    }

    std::string group_b;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(group_b);
    }

    event::Command command;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(command);
    }

    stage->group_.End(group_a, group_b, command);
  }
}
}