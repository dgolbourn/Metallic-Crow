#include "script_impl.h"
#include "bind.h"
#include <algorithm>
namespace game
{
auto Script::Impl::StageGet() -> StagePtr
{
  StagePtr ptr;
  std::string name;
  lua_.Pop(name);

  if(name == stage_.first)
  {
    ptr = stage_.second;
  }
  else
  {
    auto stage = stages_.find(name);
    if(stage != stages_.end())
    {
      ptr = stage->second;
    }
  }
  return ptr;
}

auto Script::Impl::StageInit() -> void
{
  lua_.Add(function::Bind(&Impl::StageNominate, shared_from_this()), "stage_nominate", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::StageLoad, shared_from_this()), "stage_load", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::StageFree, shared_from_this()), "stage_free", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::StageAmbient, shared_from_this()), "stage_ambient", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::StagePause, shared_from_this()), "stage_pause", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::StageResume, shared_from_this()), "stage_resume", 0, "metallic_crow");
}

auto Script::Impl::StageNominate() -> void
{
  std::string name;
  {
    lua::Guard guard = lua_.Get(-1);
    lua_.Pop(name);
  }

  if(name != stage_.first)
  {
    auto stage = stages_.find(name);
    if(stage != stages_.end())
    {
      if(stage_.second)
      {
        for(auto& sound : stage_.second->sounds_)
        {
          sound.second(0.f);
        }
      }

      stage_.first = stage->first;
      stage_.second = stage->second;
      
      for(auto& sound : stage_.second->sounds_)
      {
        sound.second(volume_);
      }
      stage_.second->current_music_(volume_);

      window_.Rotation(stage_.second->angle_);
    }
  }
}

auto Script::Impl::StageLoad() -> void
{
  std::string name;
  {
    lua::Guard guard = lua_.Get(-2);
    lua_.Pop(name);
  }

  StagePtr stage = std::make_shared<Stage>();
  
  stage->collision_ = collision::Collision(queue_);

  {
    lua::Guard guard = lua_.Get(-1);
    
    {
      lua::Guard guard = lua_.Field("world");
      stage->world_ = dynamics::World(lua_, stage->collision_, queue_);
    }

    stage->paused_[0] = paused_;
    stage->paused_[1] = true;
    stage->zoom_ = 1.f;
    stage->angle_ = 0.;

    {
      lua::Guard guard = lua_.Field("collision");
      stage->group_ = collision::Group(lua_, stage->collision_);
    }

    Choice choice;
    {
      lua::Guard guard = lua_.Field("choice");
      choice = Choice(lua_, window_, queue_, path_);
    }

    choice.Up(function::Bind(&Impl::Call, shared_from_this(), "choice_up"));
    choice.Down(function::Bind(&Impl::Call, shared_from_this(), "choice_down"));
    choice.Left(function::Bind(&Impl::Call, shared_from_this(), "choice_left"));
    choice.Right(function::Bind(&Impl::Call, shared_from_this(), "choice_right"));
    choice.Timer(function::Bind(&Impl::Call, shared_from_this(), "choice_timer"));
    stage->choice_ = choice;

    {
      lua::Guard guard = lua_.Field("subtitle");
      stage->subtitle_ = Subtitle(lua_, window_, path_);
    }
  }

  stages_.emplace(name, stage);
}

auto Script::Impl::StageFree() -> void
{
  std::string name;
  {
    lua::Guard guard = lua_.Get(-1);
    lua_.Pop(name);
  }
  stages_.erase(name);
}

auto Script::Impl::StageAmbient() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-4);
    stage = StageGet();
  }
  if(stage)
  {
    float r;
    {
      lua::Guard guard = lua_.Get(-3);
      lua_.Pop(r);
    }

    float g;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(g);
    }

    float b;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(b);
    }

    stage->world_.Ambient(r, g, b);
  }
}

auto Script::Impl::StagePause() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-1);
    stage = StageGet();
  }
  if(stage)
  {
    Pause(stage, stage->paused_[1]);
  }
}

auto Script::Impl::StageResume() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-1);
    stage = StageGet();
  }
  if(stage)
  {
    Resume(stage, stage->paused_[1]);
  }
}

auto Script::Impl::Pause(StagePtr const& stage) -> bool
{
  return std::any_of(stage->paused_.begin(), stage->paused_.end(), [](bool paused){return paused;});
}

auto Script::Impl::Pause(StagePtr const& stage, bool& paused) -> void
{
  bool current = Pause(stage);
  paused = true;
  bool next = Pause(stage);

  if(next && !current)
  {
    for(auto& actor : stage->actors_)
    {
      actor.second.Pause();
    }
    for(auto& screen : stage->screens_)
    {
      screen.second.Pause();
    }
    stage->world_.Pause();
    stage->choice_.Pause();
    for(auto timer = stage->timers_.begin(); timer != stage->timers_.end();)
    {
      if(timer->second)
      {
        timer->second.Pause();
        ++timer;
      }
      else
      {
        timer = stage->timers_.erase(timer);
      }
    }
    for(auto& sound : stage->sounds_)
    {
      sound.second.Pause();
    }
    for(auto& music : stage->music_)
    {
      music.second.Pause();
    }
  }
}

auto Script::Impl::Resume(StagePtr const& stage, bool& paused) -> void
{
  bool current = Pause(stage);
  paused = false;
  bool next = Pause(stage);

  if(current && !next)
  {
    for(auto& actor : stage->actors_)
    {
      actor.second.Resume();
    }
    for(auto& screen : stage->screens_)
    {
      screen.second.Resume();
    }
    stage->world_.Resume();
    stage->choice_.Resume();
    for(auto timer = stage->timers_.begin(); timer != stage->timers_.end();)
    {
      if(timer->second)
      {
        timer->second.Resume();
        ++timer;
      }
      else
      {
        timer = stage->timers_.erase(timer);
      }
    }
    for(auto& sound : stage->sounds_)
    {
      sound.second.Resume();
    }
    for(auto& music : stage->music_)
    {
      music.second.Resume();
    }
  }
}
}