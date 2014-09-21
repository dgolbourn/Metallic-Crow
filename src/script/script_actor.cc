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
  lua_.Add(function::Bind(&Impl::ActorNominate, shared_from_this()), "actor_nominate", 0);
  lua_.Add(function::Bind(&Impl::ActorPosition, shared_from_this()), "actor_position", 0);
  lua_.Add(function::Bind(&Impl::ActorVelocity, shared_from_this()), "actor_velocity", 0);
  lua_.Add(function::Bind(&Impl::ActorForce, shared_from_this()), "actor_force", 0);
  lua_.Add(function::Bind(&Impl::ActorImpulse, shared_from_this()), "actor_impulse", 0);
  lua_.Add(function::Bind(&Impl::ActorUp, shared_from_this()), "actor_up", 0);
  lua_.Add(function::Bind(&Impl::ActorDown, shared_from_this()), "actor_down", 0);
  lua_.Add(function::Bind(&Impl::ActorLeft, shared_from_this()), "actor_left", 0);
  lua_.Add(function::Bind(&Impl::ActorRight, shared_from_this()), "actor_right", 0);
}

void Script::Impl::ActorLoad()
{
  std::string name;
  std::string file;
  lua_.PopFront(name);
  lua_.PopFront(file);
  Actor actor(json::JSON(file), window_, stage_->scene_, stage_->group_, queue_, stage_->world_);
  if(!Pause(stage_))
  {
    actor.Resume();
  }
  stage_->actors_.emplace(name, actor);
}

void Script::Impl::ActorFree()
{
  std::string name;
  lua_.PopFront(name);
  stage_->actors_.erase(name);
}

void Script::Impl::ActorBody()
{
  std::string name;
  std::string expression;
  lua_.PopFront(name);
  lua_.PopFront(expression);
  auto range = stage_->actors_.equal_range(name);
  for(auto& actor = range.first; actor != range.second; ++actor)
  {
    actor->second.Body(expression);
  }
}

void Script::Impl::ActorEyes()
{
  std::string name;
  std::string expression;
  lua_.PopFront(name);
  lua_.PopFront(expression);
  auto range = stage_->actors_.equal_range(name);
  for(auto& actor = range.first; actor != range.second; ++actor)
  {
    actor->second.Eyes(expression);
  }
}

void Script::Impl::ActorMouth()
{
  std::string name;
  std::string expression;
  lua_.PopFront(name);
  lua_.PopFront(expression);
  auto range = stage_->actors_.equal_range(name);
  for(auto& actor = range.first; actor != range.second; ++actor)
  {
    actor->second.Mouth(expression);
  }
}

void Script::Impl::ActorNominate()
{
  std::string name;
  lua_.PopFront(name);
  
  Actor hero;
  auto iter = stage_->actors_.find(name);
  if(iter != stage_->actors_.end())
  {
    hero = iter->second;
  }

  stage_->hero_ = hero;
}

void Script::Impl::ActorPosition()
{
  std::string name;
  float x;
  float y;
  lua_.PopFront(name);
  lua_.PopFront(x);
  lua_.PopFront(y);
  auto range = stage_->actors_.equal_range(name);
  for(auto& actor = range.first; actor != range.second; ++actor)
  {
    actor->second.Position(Position(x, y));
  }
}

void Script::Impl::ActorVelocity()
{
  std::string name;
  float u;
  float v;
  lua_.PopFront(name);
  lua_.PopFront(u);
  lua_.PopFront(v);
  auto range = stage_->actors_.equal_range(name);
  for(auto& actor = range.first; actor != range.second; ++actor)
  {
    actor->second.Velocity(Position(u, v));
  }
}

void Script::Impl::ActorForce()
{
  std::string name;
  float f;
  float g;
  lua_.PopFront(name);
  lua_.PopFront(f);
  lua_.PopFront(g);
  auto range = stage_->actors_.equal_range(name);
  for(auto& actor = range.first; actor != range.second; ++actor)
  {
    actor->second.Force(Position(f, g));
  }
}

void Script::Impl::ActorImpulse()
{
  std::string name;
  float i;
  float j;
  lua_.PopFront(name);
  lua_.PopFront(i);
  lua_.PopFront(j);
  auto range = stage_->actors_.equal_range(name);
  for(auto& actor = range.first; actor != range.second; ++actor)
  {
    actor->second.Impulse(Position(i, j));
  }
}

void Script::Impl::ActorUp()
{
  std::string name;
  lua_.PopFront(name);
  auto range = stage_->actors_.equal_range(name);
  for(auto& actor = range.first; actor != range.second; ++actor)
  {
    actor->second.Up();
  }
}

void Script::Impl::ActorDown()
{
  std::string name;
  lua_.PopFront(name);
  auto range = stage_->actors_.equal_range(name);
  for(auto& actor = range.first; actor != range.second; ++actor)
  {
    actor->second.Down();
  }
}

void Script::Impl::ActorLeft()
{
  std::string name;
  lua_.PopFront(name);
  auto range = stage_->actors_.equal_range(name);
  for(auto& actor = range.first; actor != range.second; ++actor)
  {
    actor->second.Left();
  }
}

void Script::Impl::ActorRight()
{
  std::string name;
  lua_.PopFront(name);
  auto range = stage_->actors_.equal_range(name);
  for(auto& actor = range.first; actor != range.second; ++actor)
  {
    actor->second.Right();
  }
}
}