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
  lua_.Add(function::Bind(&Impl::ActorPlane, shared_from_this()), "actor_plane", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ActorScale, shared_from_this()), "actor_scale", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ActorLink, shared_from_this()), "actor_link", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ActorUnlink, shared_from_this()), "actor_unlink", 0, "metallic_crow");
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
      actor = Actor(lua_, window_, stage->collision_, queue_, stage->world_, path_);
      {
        lua::Guard guard = lua_.Field("game_body");
        if(lua_.Check())
        {
          lua::Guard guard = lua_.Field("plane");
          if(lua_.Check())
          {
            stage->scene_.left.insert(Scene::left_value_type(lua_.At<float>(-1), actor));
          }
        }
      }
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
    actor.first->scene_.right.erase(actor.second);
    actor.first->subjects_.erase(actor.second);
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
    std::string expression = lua_.At<std::string>(-2);
    int facing = lua_.At<int>(-1);

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
    actor.Eyes(lua_.At<std::string>(-1));
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
    actor.Mouth(lua_.At<std::string>(-1));
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
    actor.Position(std::make_pair(lua_.At<float>(-2), lua_.At<float>(-1)));
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
    actor.Velocity(std::make_pair(lua_.At<float>(-2), lua_.At<float>(-1)));
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
    actor.Force(std::make_pair(lua_.At<float>(-2), lua_.At<float>(-1)));
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
    actor.Impulse(std::make_pair(lua_.At<float>(-2), lua_.At<float>(-1)));
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
    actor.Modulation(lua_.At<float>(-4), lua_.At<float>(-3), lua_.At<float>(-2), lua_.At<float>(-1));
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
    actor.Dilation(lua_.At<float>(-1));
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
    actor.Rotation(lua_.At<double>(-1));
  }
}

auto Script::Impl::ActorScale() -> void
{
  Actor actor;
  {
    lua::Guard guard = lua_.Get(-2);
    actor = DataGet<Actor>();
  }
  if(actor)
  {
    actor.Scale(lua_.At<float>(-1));
  }
}

auto Script::Impl::ActorPlane() -> void
{
  std::pair<StagePtr, Actor> actor;
  {
    lua::Guard guard = lua_.Get(-2);
    actor = StageDataGet<Actor>();
  }
  if(actor.first && actor.second)
  {
    actor.first->scene_.right.replace_data(actor.first->scene_.right.find(actor.second), lua_.At<float>(-1));
  }
}

auto Script::Impl::ActorLink() -> void
{
  Actor actor;
  {
    lua::Guard guard = lua_.Get(-2);
    actor = DataGet<Actor>();
  }
  if(actor)
  {
    actor.Link(lua_.At<std::string>(-1));
  }
}

auto Script::Impl::ActorUnlink() -> void
{
  Actor actor;
  {
    lua::Guard guard = lua_.Get(-2);
    actor = DataGet<Actor>();
  }
  if(actor)
  {
    actor.Unlink(lua_.At<std::string>(-1));
  }
}
}