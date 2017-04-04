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
  lua_.Add(function::Bind(&Impl::ActorEmit, shared_from_this()), "actor_emit", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ActorIntrinsic, shared_from_this()), "actor_intrinsic", 0, "metallic_crow");
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

      Position order_position;
      {
        lua::Guard guard = lua_.Field("order_position");
        if(lua_.Check())
        {
          Position position = actor.Position();
          order_position.first = lua_.Field<float>(1) - position.first;
          order_position.second = lua_.Field<float>(2) - position.second;
        }
      }

      bool visible = false;
      {
        lua::Guard guard = lua_.Field("game_body");
        if(lua_.Check())
        {
          lua::Guard guard = lua_.Field("plane");
          if(lua_.Check())
          {
            Position position = actor.Position();
            Order order = {lua_.At<float>(-1), position.second + order_position.second, position.first + order_position.first};
            stage->scene_.left.insert(Scene::left_value_type(order, actor, order_position));
            visible = true;
          }
        }
      }
      if(visible)
      {
        lua::Guard guard = lua_.Field("dynamics_body");
        if(lua_.Check())
        {
          lua::Guard guard = lua_.Field("body");
          if(lua_.Check())
          {
            lua::Guard guard = lua_.Field("velocity");
            if(lua_.Check())
            {
               stage->actives_.emplace(actor);
            }
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
    actor.first->actives_.erase(actor.second);
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
    int direction = lua_.At<int>(-1);

    if(expression == "")
    {
      if(direction)
      {
        actor.Body(direction);
      }
    }
    else if(direction)
    {
      actor.Body(expression, direction);
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
  std::pair<StagePtr, Actor> actor;
  {
    lua::Guard guard = lua_.Get(-3);
    actor = StageDataGet<Actor>();
  }
  if(actor.first && actor.second)
  {
    Position position = std::make_pair(lua_.At<float>(-2), lua_.At<float>(-1));
    actor.second.Position(position);
    auto iter = actor.first->scene_.right.find(actor.second);
    actor.first->scene_.right.modify_data(iter, 
        [&](Order& order)
        {
          order[1] = position.second + iter->info.second;
          order[2] = position.first + iter->info.first;
        });
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

auto Script::Impl::ActorIntrinsic() -> void
{
  Actor actor;
  {
    lua::Guard guard = lua_.Get(-4);
    actor = DataGet<Actor>();
  }
  if(actor)
  {
    actor.Intrinsic(lua_.At<float>(-3), lua_.At<float>(-2), lua_.At<float>(-1));
  }
}

auto Script::Impl::ActorEmit() -> void
{
  Actor actor;
  {
    lua::Guard guard = lua_.Get(-4);
    actor = DataGet<Actor>();
  }
  if(actor)
  {
    actor.Emit(lua_.At<float>(-3), lua_.At<float>(-2), lua_.At<float>(-1));
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
    actor.first->scene_.right.modify_data(actor.first->scene_.right.find(actor.second), 
        [&](Order& order)
        {
          order[0] = lua_.At<float>(-1);
        });
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