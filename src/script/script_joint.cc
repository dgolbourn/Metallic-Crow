#include "script_impl.h"
#include "bind.h"
namespace game
{
auto Script::Impl::JointInit() -> void
{
  lua::Init<std::pair<WeakStagePtr, Joint::WeakPtr>>(static_cast<lua_State*>(lua_));
  lua_.Add(function::Bind(&Impl::JointLoad, shared_from_this()), "joint_load", 1, "metallic_crow");
  lua_.Add(function::Bind(&Impl::JointFree, shared_from_this()), "joint_free", 0, "metallic_crow");
}

auto Script::Impl::JointLoad() -> void
{
  Joint joint;

  std::pair<StagePtr, Actor> actor_a;
  {
    lua::Guard guard = lua_.Get(-3);
    actor_a = StageDataGet<Actor>();
  }
  if(actor_a.first && actor_a.second)
  {    
    std::pair<StagePtr, Actor> actor_b;
    {
      lua::Guard guard = lua_.Get(-2);
      actor_b = StageDataGet<Actor>();
    }

    if((actor_b.first == actor_a.first) && actor_b.second)
    {
      lua::Guard guard = lua_.Get(-1);
      joint = Joint(lua_, actor_a.second, actor_b.second, actor_a.first->world_);
      actor_a.first->joints_.emplace(joint);
    }
  }

  lua::Push(static_cast<lua_State*>(lua_), std::pair<WeakStagePtr, Joint::WeakPtr>(actor_a.first, joint));
}

auto Script::Impl::JointFree() -> void
{
  std::pair<StagePtr, Joint> joint = StageDataGet<Joint>(); 
  if(joint.first && joint.second)
  {
    joint.first->joints_.erase(joint.second);
  }
}
}