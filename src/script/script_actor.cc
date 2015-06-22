#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::ActorInit()
{
  lua_.Add(function::Bind(&Impl::ActorLoad, shared_from_this()), "actor_load", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ActorFree, shared_from_this()), "actor_free", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ActorBody, shared_from_this()), "actor_body", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ActorEyes, shared_from_this()), "actor_eyes", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ActorMouth, shared_from_this()), "actor_mouth", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ActorPosition, shared_from_this()), "actor_position", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ActorVelocity, shared_from_this()), "actor_velocity", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ActorForce, shared_from_this()), "actor_force", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ActorImpulse, shared_from_this()), "actor_impulse", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ActorModulation, shared_from_this()), "actor_modulation", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ActorDilation, shared_from_this()), "actor_dilation", 0, "metallic_crow");
}

void Script::Impl::ActorLoad()
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

    Actor actor;
    {
      lua::Guard guard = lua_.Get(-1);
      actor = Actor(lua_, window_, stage->scene_, stage->group_, queue_, stage->world_, path_);
    }
  
    if(!Pause(stage))
    {
      actor.Resume();
    }
    stage->actors_.emplace(name, actor);
  }
}

void Script::Impl::ActorFree()
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
    stage->actors_.erase(name);
  }
}

void Script::Impl::ActorBody()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-4);
    stage = StageGet();
  }
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-3);
      lua_.Pop(name);
    }

    std::string expression;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(expression);
    }

    int facing;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(facing);
    }

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
    std::string expression;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(expression);
    }

    auto range = stage->actors_.equal_range(name);
    for(auto& actor = range.first; actor != range.second; ++actor)
    {
      actor->second.Eyes(expression);
    }
  }
}

void Script::Impl::ActorMouth()
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
    std::string expression;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(expression);
    }

    auto range = stage->actors_.equal_range(name);
    for(auto& actor = range.first; actor != range.second; ++actor)
    {
      actor->second.Mouth(expression);
    }
  }
}

void Script::Impl::ActorPosition()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-4);
    stage = StageGet();
  }
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-3);
      lua_.Pop(name);
    }

    Position position;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(position.first);
    }
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(position.second);
    }

    auto range = stage->actors_.equal_range(name);
    for(auto& actor = range.first; actor != range.second; ++actor)
    {
      actor->second.Position(position);
    }
  }
}

void Script::Impl::ActorVelocity()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-4);
    stage = StageGet();
  }
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-3);
      lua_.Pop(name);
    }

    Position velocity;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(velocity.first);
    }
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(velocity.second);
    }

    auto range = stage->actors_.equal_range(name);
    for(auto& actor = range.first; actor != range.second; ++actor)
    {
      actor->second.Velocity(velocity);
    }
  }
}

void Script::Impl::ActorForce()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-4);
    stage = StageGet();
  }
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-3);
      lua_.Pop(name);
    }

    Position force;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(force.first);
    }
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(force.second);
    }

    auto range = stage->actors_.equal_range(name);
    for(auto& actor = range.first; actor != range.second; ++actor)
    {
      actor->second.Force(force);
    }
  }
}

void Script::Impl::ActorImpulse()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-4);
    stage = StageGet();
  }
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-3);
      lua_.Pop(name);
    }

    Position impulse;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(impulse.first);
    }
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(impulse.second);
    }

    auto range = stage->actors_.equal_range(name);
    for(auto& actor = range.first; actor != range.second; ++actor)
    {
      actor->second.Impulse(impulse);
    }
  }
}

void Script::Impl::ActorModulation()
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

    auto range = stage->actors_.equal_range(name);
    for(auto& actor = range.first; actor != range.second; ++actor)
    {
      actor->second.Modulation(r, g, b, a);
    }
  }
}

void Script::Impl::ActorDilation()
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
    float dilation;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(dilation);
    }
    auto range = stage->actors_.equal_range(name);
    for(auto& actor = range.first; actor != range.second; ++actor)
    {
      actor->second.Dilation(dilation);
    }
  }
}
}