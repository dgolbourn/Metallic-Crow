#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::ActorInit()
{
  lua_.Add(function::Bind(&Impl::ActorLoad, shared_from_this()), "actor_load", 0);
  lua_.Add(function::Bind(&Impl::ActorFree, shared_from_this()), "actor_free", 0);
  lua_.Add(function::Bind(&Impl::ActorBody, shared_from_this()), "actor_body", 0);
  lua_.Add(function::Bind(&Impl::ActorEyes, shared_from_this()), "actor_eyes", 0);
  lua_.Add(function::Bind(&Impl::ActorMouth, shared_from_this()), "actor_mouth", 0);
  lua_.Add(function::Bind(&Impl::ActorPosition, shared_from_this()), "actor_position", 0);
  lua_.Add(function::Bind(&Impl::ActorVelocity, shared_from_this()), "actor_velocity", 0);
  lua_.Add(function::Bind(&Impl::ActorForce, shared_from_this()), "actor_force", 0);
  lua_.Add(function::Bind(&Impl::ActorImpulse, shared_from_this()), "actor_impulse", 0);
}

void Script::Impl::ActorLoad()
{
  StagePtr stage = StagePop();
  std::string name;
  std::string file;
  lua_.PopFront(name);
  lua_.PopFront(file);
  if(stage)
  {
    Actor actor(json::JSON(path_ / file), window_, stage->scene_, stage->group_, queue_, stage->world_, path_);
    if(!Pause(stage))
    {
      actor.Resume();
    }
    stage->actors_.emplace(name, actor);
  }
}

void Script::Impl::ActorFree()
{
  StagePtr stage = StagePop();
  std::string name;
  lua_.PopFront(name);
  if(stage)
  {
    stage->actors_.erase(name);
  }
}

void Script::Impl::ActorBody()
{
  StagePtr stage = StagePop();
  std::string name;
  std::string expression;
  int facing;
  lua_.PopFront(name);
  lua_.PopFront(expression);
  lua_.PopFront(facing);
  if(stage)
  {
    auto range = stage->actors_.equal_range(name);
    for(auto& actor = range.first; actor != range.second; ++actor)
    {
      if(expression == "")
      {
        if(facing)
        {
          actor->second.Body(facing < 0);
        }
      }
      else if(facing)
      {
        actor->second.Body(expression, facing < 0);
      }
      else
      {
        actor->second.Body(expression);
      }
    }
  }
}

void Script::Impl::ActorEyes()
{
  StagePtr stage = StagePop();
  std::string name;
  std::string expression;
  lua_.PopFront(name);
  lua_.PopFront(expression);
  if(stage)
  {
    auto range = stage->actors_.equal_range(name);
    for(auto& actor = range.first; actor != range.second; ++actor)
    {
      actor->second.Eyes(expression);
    }
  }
}

void Script::Impl::ActorMouth()
{
  StagePtr stage = StagePop();
  std::string name;
  std::string expression;
  lua_.PopFront(name);
  lua_.PopFront(expression);
  if(stage)
  {
    auto range = stage->actors_.equal_range(name);
    for(auto& actor = range.first; actor != range.second; ++actor)
    {
      actor->second.Mouth(expression);
    }
  }
}

void Script::Impl::ActorPosition()
{
  StagePtr stage = StagePop();
  std::string name;
  float x;
  float y;
  lua_.PopFront(name);
  lua_.PopFront(x);
  lua_.PopFront(y);
  if(stage)
  {
    auto range = stage->actors_.equal_range(name);
    for(auto& actor = range.first; actor != range.second; ++actor)
    {
      actor->second.Position(Position(x, y));
    }
  }
}

void Script::Impl::ActorVelocity()
{
  StagePtr stage = StagePop();
  std::string name;
  float u;
  float v;
  lua_.PopFront(name);
  lua_.PopFront(u);
  lua_.PopFront(v);
  if(stage)
  {
    auto range = stage->actors_.equal_range(name);
    for(auto& actor = range.first; actor != range.second; ++actor)
    {
      actor->second.Velocity(Position(u, v));
    }
  }
}

void Script::Impl::ActorForce()
{
  StagePtr stage = StagePop();
  std::string name;
  float f;
  float g;
  lua_.PopFront(name);
  lua_.PopFront(f);
  lua_.PopFront(g);
  if(stage)
  {
    auto range = stage->actors_.equal_range(name);
    for(auto& actor = range.first; actor != range.second; ++actor)
    {
      actor->second.Force(Position(f, g));
    }
  }
}

void Script::Impl::ActorImpulse()
{
  StagePtr stage = StagePop();
  std::string name;
  float i;
  float j;
  lua_.PopFront(name);
  lua_.PopFront(i);
  lua_.PopFront(j);
  if(stage)
  {
    auto range = stage->actors_.equal_range(name);
    for(auto& actor = range.first; actor != range.second; ++actor)
    {
      actor->second.Impulse(Position(i, j));
    }
  }
}
}