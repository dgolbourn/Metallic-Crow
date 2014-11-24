#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::JointInit()
{
  lua_.Add(function::Bind(&Impl::JointLoad, shared_from_this()), "joint_load", 0);
  lua_.Add(function::Bind(&Impl::JointFree, shared_from_this()), "joint_free", 0);
}

void Script::Impl::JointLoad()
{
  StagePtr stage = StagePop();
  std::string name;
  lua_.PopFront(name);
  std::string file;
  lua_.PopFront(file);
  std::string actor_a_name;
  lua_.PopFront(actor_a_name);
  std::string actor_b_name;
  lua_.PopFront(actor_b_name);

  if(stage)
  {
    json::JSON json(path_ / file);
    auto range_a = stage->actors_.equal_range(actor_a_name);
    auto range_b = stage->actors_.equal_range(actor_b_name);
    for(auto& actor_a = range_a.first; actor_a != range_a.second; ++actor_a)
    {
      for(auto& actor_b = range_b.first; actor_b != range_b.second; ++actor_b)
      {
        stage->joints_.emplace(name, Joint(json, actor_a->second, actor_b->second, stage->world_));
      }
    }
  }
}

void Script::Impl::JointFree()
{
  StagePtr stage = StagePop();
  std::string name;
  lua_.PopFront(name);
  if(stage)
  {
    stage->joints_.erase(name);
  }
}
}