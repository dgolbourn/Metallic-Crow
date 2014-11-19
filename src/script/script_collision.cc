#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::CollisionInit()
{
  lua_.Add(function::Bind(&Impl::CollisionBegin, shared_from_this()), "collision_begin", 0);
  lua_.Add(function::Bind(&Impl::CollisionEnd, shared_from_this()), "collision_end", 0);
}

void Script::Impl::CollisionBegin()
{
  std::string group_a;
  std::string group_b;
  event::Command command;
  lua_.PopFront(group_a);
  lua_.PopFront(group_b);
  lua_.PopFront(command);
  stage_->group_.Begin(group_a, group_b, command);
}

void Script::Impl::CollisionEnd()
{
  std::string group_a;
  std::string group_b;
  event::Command command;
  lua_.PopFront(group_a);
  lua_.PopFront(group_b);
  lua_.PopFront(command);
  stage_->group_.End(group_a, group_b, command);
}
}