#include "script_impl.h"
#include "bind.h"
#include <algorithm>
namespace game
{
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
  auto stage = stages_.find(name);
  if(stage != stages_.end())
  {
    stage_ = stage->second;
  }
}

void Script::Impl::StageLoad()
{
  std::string name;
  std::string world_file;
  std::string scene;
  std::string dialogue_file;
  std::string collision;
  lua_.PopFront(name);
  lua_.PopFront(world_file);
  lua_.PopFront(scene);
  lua_.PopFront(dialogue_file);
  lua_.PopFront(collision);
  
  StagePtr stage = std::make_shared<Stage>();
  
  dynamics::World world(json::JSON(world_file), stage->collision_, queue_);
  world.End(function::Bind(&Impl::View, shared_from_this(), dynamics::World::WeakPtr(world)));
  stage->world_ = world;

  stage->scene_ = Scene(json::JSON(scene), window_);

  Subtitle dialogue(json::JSON(dialogue_file), window_, queue_);
  dialogue.Up(function::Bind(&Impl::Call, shared_from_this(), "dialogue_up"));
  dialogue.Down(function::Bind(&Impl::Call, shared_from_this(), "dialogue_down"));
  dialogue.Left(function::Bind(&Impl::Call, shared_from_this(), "dialogue_left"));
  dialogue.Right(function::Bind(&Impl::Call, shared_from_this(), "dialogue_right"));
  dialogue.Timer(function::Bind(&Impl::Call, shared_from_this(), "dialogue_timer"));
  stage_->dialogue_ = dialogue;

  stage->group_ = collision::Group(json::JSON(collision), stage->collision_);
  
  stage->paused_[0] = paused_;
  stage->paused_[1] = true;
  stage->zoom_ = 1.f;

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
  std::string name;
  float r, g, b;
  lua_.PopFront(name);
  lua_.PopFront(r);
  lua_.PopFront(g);
  lua_.PopFront(b);

  auto stage = stages_.find(name);
  if(stage != stages_.end())
  {
    stage->second->scene_.Modulation(r, g, b);
    stage->second->world_.Ambient(r, g, b);
  }
}

void Script::Impl::StagePause()
{
  std::string name;
  lua_.PopFront(name);

  auto stage = stages_.find(name);
  if(stage != stages_.end())
  {
    Pause(stage->second, stage->second->paused_[1]);
  }
}

void Script::Impl::StageResume()
{
  std::string name;
  lua_.PopFront(name);

  auto stage = stages_.find(name);
  if(stage != stages_.end())
  {
    Resume(stage->second, stage->second->paused_[1]);
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
    stage->world_.Pause();
    stage->dialogue_.Pause();
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
    stage->world_.Resume();
    stage->dialogue_.Resume();
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
  }
}
}