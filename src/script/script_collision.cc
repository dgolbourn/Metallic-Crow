#include "script_impl.h"
#include "bind.h"
namespace game
{
auto Script::Impl::CollisionInit() -> void
{
  lua_.Add(function::Bind(&Impl::CollisionBegin, shared_from_this()), "collision_begin", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::CollisionEnd, shared_from_this()), "collision_end", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::CollisionLink, shared_from_this()), "collision_link", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::CollisionUnlink, shared_from_this()), "collision_unlink", 0, "metallic_crow");
}

auto Script::Impl::CollisionBegin() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-4);
    stage = StageGet();
  }
  if(stage)
  {
    stage->collision_.Begin(lua_.At<std::string>(-3), lua_.At<std::string>(-2), lua_.At<event::Command>(-1));
  }
}

auto Script::Impl::CollisionEnd() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-4);
    stage = StageGet();
  }
  if(stage)
  {
    stage->collision_.End(lua_.At<std::string>(-3), lua_.At<std::string>(-2), lua_.At<event::Command>(-1));
  }
}

auto Script::Impl::CollisionLink() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-3);
    stage = StageGet();
  }
  if(stage)
  {
    stage->collision_.Link(lua_.At<std::string>(-2), lua_.At<std::string>(-1));
  }
}

auto Script::Impl::CollisionUnlink() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-3);
    stage = StageGet();
  }
  if(stage)
  {
    stage->collision_.Unlink(lua_.At<std::string>(-2), lua_.At<std::string>(-1));
  }
}
}