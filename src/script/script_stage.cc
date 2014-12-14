#include "script_impl.h"
#include "bind.h"
#include <algorithm>
namespace game
{
StagePtr Script::Impl::StagePop()
{
  StagePtr ptr;
  std::string name;
  lua_.PopFront(name);
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

void Script::Impl::StageInit()
{
  lua_.Add(function::Bind(&Impl::StageNominate, shared_from_this()), "stage_nominate", 0);
  lua_.Add(function::Bind(&Impl::StageLoad, shared_from_this()), "stage_load", 0);
  lua_.Add(function::Bind(&Impl::StageFree, shared_from_this()), "stage_free", 0);
  lua_.Add(function::Bind(&Impl::StageLight, shared_from_this()), "stage_light", 0);
  lua_.Add(function::Bind(&Impl::StagePause, shared_from_this()), "stage_pause", 0);
  lua_.Add(function::Bind(&Impl::StageResume, shared_from_this()), "stage_resume", 0);
}

void Script::Impl::StageNominate()
{
  std::string name;
  lua_.PopFront(name);
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
    }
  }
}

void Script::Impl::StageLoad()
{
  std::string name;
  std::string world_file;
  std::string choice_file;
  std::string collision_file;
  std::string subtitle_file;
  lua_.PopFront(name);
  lua_.PopFront(world_file);
  lua_.PopFront(choice_file);
  lua_.PopFront(collision_file);
  lua_.PopFront(subtitle_file);
  
  StagePtr stage = std::make_shared<Stage>();
  
  stage->collision_ = collision::Collision(queue_);

  stage->world_ = dynamics::World(json::JSON(path_ / world_file), stage->collision_, queue_);

  stage->paused_[0] = paused_;
  stage->paused_[1] = true;
  stage->zoom_ = 1.f;

  stage->group_ = collision::Group(json::JSON(path_ / collision_file), stage->collision_);

  Choice choice(json::JSON(path_ / choice_file), window_, queue_, path_);
  choice.Up(function::Bind(&Impl::Call, shared_from_this(), "choice_up"));
  choice.Down(function::Bind(&Impl::Call, shared_from_this(), "choice_down"));
  choice.Left(function::Bind(&Impl::Call, shared_from_this(), "choice_left"));
  choice.Right(function::Bind(&Impl::Call, shared_from_this(), "choice_right"));
  choice.Timer(function::Bind(&Impl::Call, shared_from_this(), "choice_timer"));
  stage->choice_ = choice;

  stage->subtitle_ = Subtitle(json::JSON(path_ / subtitle_file), window_, path_);

  stages_.emplace(name, stage);
}

void Script::Impl::StageFree()
{
  std::string name;
  lua_.PopFront(name);
  stages_.erase(name);
}

void Script::Impl::StageLight()
{
  StagePtr stage = StagePop();
  float r, g, b;
  lua_.PopFront(r);
  lua_.PopFront(g);
  lua_.PopFront(b);
  if(stage)
  {
    stage->world_.Ambient(r, g, b);
  }
}

void Script::Impl::StagePause()
{
  StagePtr stage = StagePop();
  if(stage)
  {
    Pause(stage, stage->paused_[1]);
  }
}

void Script::Impl::StageResume()
{
  StagePtr stage = StagePop();
  if(stage)
  {
    Resume(stage, stage->paused_[1]);
  }
}

bool Script::Impl::Pause(StagePtr const& stage)
{
  return std::any_of(stage->paused_.begin(), stage->paused_.end(), [](bool paused){return paused;});
}

void Script::Impl::Pause(StagePtr const& stage, bool& paused)
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

void Script::Impl::Resume(StagePtr const& stage, bool& paused)
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