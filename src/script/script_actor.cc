#include "script_impl.h"
#include "bind.h"
namespace game
{
auto Script::Impl::ActorInit() -> void
{
  lua::Init<std::pair<WeakStagePtr, Actor::WeakPtr>>(static_cast<lua_State*>(lua_));
  lua_.Add(function::Bind(&Impl::ActorLoad, shared_from_this()), "actor_load", 1, "metallic_crow");
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
  lua_.Add(function::Bind(&Impl::ActorRotation, shared_from_this()), "actor_rotation", 0, "metallic_crow");
}

auto Script::Impl::ActorLoad() -> void
{
  Actor actor;
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-2);
    stage = StageGet();
  }
  if(stage)
  {
    {
      lua::Guard guard = lua_.Get(-1);
      actor = Actor(lua_, window_, stage->scene_, stage->group_, queue_, stage->world_, path_);
    }
  
    if(!Pause(stage))
    {
      actor.Resume();
    }

    stage->actors_.emplace(actor);
  }
  lua::Push(static_cast<lua_State*>(lua_), std::pair<WeakStagePtr, Actor::WeakPtr>(stage, actor));
}

auto Script::Impl::ActorFree() -> void
{  
  std::pair<StagePtr, Actor> actor = StageDataGet<Actor>();
  if(actor.first && actor.second)
  {
    actor.first->actors_.erase(actor.second);
  }
}

auto Script::Impl::ActorBody() -> void
{
  Actor actor;
  {
    lua::Guard guard = lua_.Get(-3);
    actor = DataGet<Actor>();
  }
  if(actor)
  {
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

    if(expression == "")
    {
      if(facing)
      {
        actor.Body(facing < 0);
      }
    }
    else if(facing)
    {
      actor.Body(expression, facing < 0);
    }
    else
    {
      actor.Body(expression);
    }
  }
}

auto Script::Impl::ActorEyes() -> void
{
  Actor actor;
  {
    lua::Guard guard = lua_.Get(-2);
    actor = DataGet<Actor>();
  }
  if(actor)
  {
    std::string expression;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(expression);
    }
    actor.Eyes(expression);
  }
}

auto Script::Impl::ActorMouth() -> void
{
  Actor actor;
  {
    lua::Guard guard = lua_.Get(-2);
    actor = DataGet<Actor>();
  }
  if(actor)
  {
    std::string expression;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(expression);
    }

    actor.Mouth(expression);
  }
}

auto Script::Impl::ActorPosition() -> void
{
  Actor actor;
  {
    lua::Guard guard = lua_.Get(-3);
    actor = DataGet<Actor>();
  }
  if(actor)
  {
    Position position;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(position.first);
    }
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(position.second);
    }

    actor.Position(position);
  }
}

auto Script::Impl::ActorVelocity() -> void
{
  Actor actor;
  {
    lua::Guard guard = lua_.Get(-3);
    actor = DataGet<Actor>();
  }
  if(actor)
  {
    Position velocity;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(velocity.first);
    }
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(velocity.second);
    }

    actor.Velocity(velocity);
  }
}

auto Script::Impl::ActorForce() -> void
{
  Actor actor;
  {
    lua::Guard guard = lua_.Get(-3);
    actor = DataGet<Actor>();
  }
  if(actor)
  {
    Position force;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(force.first);
    }
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(force.second);
    }

    actor.Force(force);
  }
}

auto Script::Impl::ActorImpulse() -> void
{
  Actor actor;
  {
    lua::Guard guard = lua_.Get(-3);
    actor = DataGet<Actor>();
  }
  if(actor)
  {
    Position impulse;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(impulse.first);
    }
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(impulse.second);
    }
    actor.Impulse(impulse);
  }
}

auto Script::Impl::ActorModulation() -> void
{
  Actor actor;
  {
    lua::Guard guard = lua_.Get(-5);
    actor = DataGet<Actor>();
  }
  if(actor)
  {
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
    actor.Modulation(r, g, b, a);
  }
}

auto Script::Impl::ActorDilation() -> void
{
  Actor actor;
  {
    lua::Guard guard = lua_.Get(-2);
    actor = DataGet<Actor>();
  }
  if(actor)
  {
    float dilation;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(dilation);
    }
    actor.Dilation(dilation);
  }
}

auto Script::Impl::ActorRotation() -> void
{
  Actor actor;
  {
    lua::Guard guard = lua_.Get(-2);
    actor = DataGet<Actor>();
  }
  if(actor)
  {
    double angle;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(angle);
    }
    actor.Rotation(angle);
  }
}
}