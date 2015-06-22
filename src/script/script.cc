#include "script_impl.h"
#include "bind.h"
#include "exception.h"
namespace game
{
Script::Impl::Impl(display::Window& window, event::Queue& queue, boost::filesystem::path const& path, float volume) : window_(window), queue_(queue), paused_(true), fade_(queue, window), begun_(false), path_(path), lua_(path), volume_(volume)
{
}

namespace
{
int step_size = 1;
double step_time = 0.1;
}

void Script::Impl::Init(boost::filesystem::path const& file)
{
  collect_ = event::Timer(step_time, -1);
  collect_.Add(function::Bind(&Impl::Collect, shared_from_this()));
  queue_.Add(function::Bind(&event::Timer::operator(), collect_));

  ActorInit();
  StageInit();
  SceneryInit();
  ScreenInit();
  ChoiceInit();
  SubtitleInit();
  TimerInit();
  ViewInit();
  CollisionInit();
  FadeInit();
  JointInit();
  AudioInit();

  typedef void (event::Signal::*Notify)();
  lua_.Add(function::Bind((Notify)&event::Signal::operator(), signal_), "script_end", 0, "metallic_crow");

  lua_.Load(file);
}

void Script::Impl::Call(std::string const& call)
{
  lua::Guard guard0 = lua_.Get("package");
  lua::Guard guard1 = lua_.Field("loaded");
  lua::Guard guard2 = lua_.Field("metallic_crow");
  lua::Guard guard3 = lua_.Field(call);
  if(lua_.Check())
  {
    lua_.Call(0, 0);
  }
}

void Script::Impl::Collect()
{
  lua_.Collect(step_size);
}

void Script::Impl::Pause()
{
  if(!paused_)
  {
    paused_ = true;
    for(auto& stage : stages_)
    {
      Pause(stage.second, stage.second->paused_[0]);
    }
    fade_.Pause();
    collect_.Pause();
  }
}

void Script::Impl::Resume()
{
  std::shared_ptr<Impl> guard = shared_from_this(); 

  if(!begun_)
  {
    begun_ = true;
    Call("script_begin");
  }

  if(paused_)
  {
    paused_ = false;
    for(auto& stage : stages_)
    {
      Resume(stage.second, stage.second->paused_[0]);
    }
    fade_.Resume();
    collect_.Resume();
  }
}

void Script::Impl::Render()
{
  if(stage_.second)
  {
    View();
    stage_.second->scene_.Render();
    stage_.second->choice_.Render();
    stage_.second->subtitle_.Render();
  }
  fade_.Render();
}

void Script::Impl::ChoiceUp()
{
  if(stage_.second)
  {
    stage_.second->choice_.Up();
  }
}

void Script::Impl::ChoiceDown()
{
  if(stage_.second)
  {
    stage_.second->choice_.Down();
  }
}

void Script::Impl::ChoiceLeft()
{
  if(stage_.second)
  {
    stage_.second->choice_.Left();
  }
}

void Script::Impl::ChoiceRight()
{
  if(stage_.second)
  {
    stage_.second->choice_.Right();
  }
}

void Script::Impl::Control(float x, float y)
{
  lua::Guard guard0 = lua_.Get("package");
  lua::Guard guard1 = lua_.Field("loaded");
  lua::Guard guard2 = lua_.Field("metallic_crow");
  lua::Guard guard3 = lua_.Field("control");
  if(lua_.Check())
  {
    lua::Guard guard4 = lua_.Push(x);
    lua::Guard guard5 = lua_.Push(y);
    lua_.Call(2, 0);
  }
}

void Script::Impl::Add(event::Command const& command)
{
  signal_.Add(command);
}

void Script::Pause()
{
  impl_->Pause();
}

void Script::Resume()
{
  impl_->Resume();
}

void Script::Render()
{
  impl_->Render();
}

void Script::ChoiceUp()
{
  impl_->ChoiceUp();
}

void Script::ChoiceDown()
{
  impl_->ChoiceDown();
}

void Script::ChoiceLeft()
{
  impl_->ChoiceLeft();
}

void Script::ChoiceRight()
{
  impl_->ChoiceRight();
}

void Script::Control(float x, float y)
{
  impl_->Control(x, y);
}

void Script::Add(event::Command const& command)
{
  impl_->Add(command);
}

Script::operator bool() const
{
  return bool(impl_);
}

Script::Script(boost::filesystem::path const& file, display::Window& window, event::Queue& queue, boost::filesystem::path const& path, float volume) : impl_(std::make_shared<Impl>(window, queue, path, volume))
{
  impl_->Init(file);
}
}