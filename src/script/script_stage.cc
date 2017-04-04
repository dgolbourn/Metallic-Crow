#include "script_impl.h"
#include "bind.h"
#include <algorithm>
#include "lua_data.h"
#include <boost/bimap/support/lambda.hpp>
namespace game
{
auto Script::Impl::StageGet() -> StagePtr
{
  WeakStagePtr ptr;
  lua::Get(static_cast<lua_State*>(lua_), ptr);
  return ptr.lock();
}

auto Script::Impl::StageInit() -> void
{
  lua::Init<WeakStagePtr>(static_cast<lua_State*>(lua_));
  lua_.Add(function::Bind(&Impl::StageNominate, shared_from_this()), "stage_nominate", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::StageLoad, shared_from_this()), "stage_load", 1, "metallic_crow");
  lua_.Add(function::Bind(&Impl::StageFree, shared_from_this()), "stage_free", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::StageAmbient, shared_from_this()), "stage_ambient", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::StagePause, shared_from_this()), "stage_pause", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::StageResume, shared_from_this()), "stage_resume", 0, "metallic_crow");
}

auto Script::Impl::StageNominate() -> void
{
  StagePtr stage = StageGet();

  if(stage)
  {
    if(stage != stage_)
    {
      if(stage_)
      {
        for(auto sound : stage_->sounds_)
        {
          sound(0.f);
        }
      }

      stage_ = stage;

      for(auto sound : stage_->sounds_)
      {
        sound(volume_);
      }

      stage_->current_music_(volume_);

      window_.Rotation(stage_->angle_);
    }
  }
}

auto Script::Impl::StageLoad() -> void
{
  StagePtr stage = std::make_shared<Stage>();

  collision::Group group(queue_);
  stage->collision_ = group;

  {
    lua::Guard guard = lua_.Field("world");
    stage->world_ = dynamics::World(lua_, group, queue_);
  }
  WeakStagePtr stage_ptr = WeakStagePtr(stage);
  stage->world_.End([=]()
  {
    if(StagePtr stage = stage_ptr.lock())
    {
      for(auto& active : stage->actives_)
      {
        auto iter = stage->scene_.right.find(active);
        stage->scene_.right.modify_data(iter, 
            [&](Order& order)
            {
              Position position = active.Position();
              order[1] = position.second + iter->info.second;
              order[2] = position.first + iter->info.first;
            });
      }
      return true;
    }
    else
    {
      return false;
    }
  });

  stage->paused_[0] = paused_;
  stage->paused_[1] = true;
  stage->zoom_ = 1.f;
  stage->angle_ = 0.;

  {
    lua::Guard guard = lua_.Field("collision");
    for(int index = 1, end = lua_.Size(); index <= end; ++index) 
    { 
      lua::Guard guard = lua_.Field(index); 
      group.Link(lua_.Field<std::string>(1), lua_.Field<std::string>(2));
    }
  }

  {
    lua::Guard guard = lua_.Field("choices");
    for(int index = 1, end = lua_.Size(); index <= end; ++index)
    {
      lua::Guard guard = lua_.Field(index);
      Choice choice(lua_, window_, queue_, path_);
      choice.Up(function::Bind(&Impl::Player, shared_from_this(), index, "choice_up"));
      choice.Down(function::Bind(&Impl::Player, shared_from_this(), index, "choice_down"));
      choice.Left(function::Bind(&Impl::Player, shared_from_this(), index, "choice_left"));
      choice.Right(function::Bind(&Impl::Player, shared_from_this(), index, "choice_right"));
      choice.Timer(function::Bind(&Impl::Player, shared_from_this(), index, "choice_timer"));
      stage->choices_[index] = choice;
    }
  }

  {
    lua::Guard guard = lua_.Field("subtitle");
    stage->subtitle_ = Subtitle(lua_, window_, path_);
  }

  {
    lua::Guard guard = lua_.Field("title");
    stage->title_ = Subtitle(lua_, window_, path_);
  }

  stages_.emplace(stage);
  lua::Push(static_cast<lua_State*>(lua_), WeakStagePtr(stage));
}

auto Script::Impl::StageFree() -> void
{
  stages_.erase(StageGet());
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
    stage->world_.Ambient(lua_.At<float>(-3), lua_.At<float>(-2), lua_.At<float>(-1));
  }
}

auto Script::Impl::StagePause() -> void
{
  StagePtr stage = StageGet();
  if(stage)
  {
    Pause(stage, stage->paused_[1]);
  }
}

auto Script::Impl::StageResume() -> void
{
  StagePtr stage = StageGet();
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
    for(auto actor : stage->actors_)
    {
      actor.Pause();
    }
    stage->world_.Pause();
    for(auto choice : stage->choices_)
    {
      choice.second.Pause();
    }
    for(auto timer : stage->timers_)
    {
      timer.Pause();
    }
    for(auto sound : stage->sounds_)
    {
      sound.Pause();
    }
    for(auto music : stage->music_)
    {
      music.Pause();
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
    for(auto actor : stage->actors_)
    {
      actor.Resume();
    }
    stage->world_.Resume();
    for(auto choice : stage->choices_)
    {
      choice.second.Resume();
    }
    for(auto timer : stage->timers_)
    {
      timer.Resume();
    }
    for(auto sound : stage->sounds_)
    {
      sound.Resume();
    }
    for(auto music : stage->music_)
    {
      music.Resume();
    }
  }
}
}