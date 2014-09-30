#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::CollisionInit()
{
  lua_.Add(function::Bind(&Impl::CollisionLink, shared_from_this()), "collision_link", 0);
  lua_.Add(function::Bind(&Impl::CollisionBegin, shared_from_this()), "collision_begin", 0);
  lua_.Add(function::Bind(&Impl::CollisionEnd, shared_from_this()), "collision_end", 0);
}

void Script::Impl::CollisionLink()
{
  std::string group_a;
  std::string group_b;
  lua_.PopFront(group_a);
  lua_.PopFront(group_b);
  stage_->group_.Link(group_a, group_b);
}

void Script::Impl::CollisionBegin()
{
  std::string group_a;
  std::string group_b;
  std::string call;
  lua_.PopFront(group_a);
  lua_.PopFront(group_b);
  lua_.PopFront(call);
  stage_->group_.Begin(group_a, group_b, function::Bind(&Impl::Call, shared_from_this(), call));
}

void Script::Impl::CollisionEnd()
{
  std::string group_a;
  std::string group_b;
  std::string call;
  lua_.PopFront(group_a);
  lua_.PopFront(group_b);
  lua_.PopFront(call);
  stage_->group_.End(group_a, group_b, function::Bind(&Impl::Call, shared_from_this(), call));
}
}