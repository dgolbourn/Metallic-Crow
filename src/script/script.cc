#include "script_impl.h"
#include "bind.h"
#include "exception.h"
namespace
{
int step_size = 1;
double step_time = 0.1;
}

namespace game
{
Script::Impl::Impl(display::Window& window, event::Queue& queue, boost::filesystem::path const& path, float volume, event::Timeslice& loader) : window_(window), queue_(queue), paused_(true), fade_(queue, window), begun_(false), path_(path), lua_(path), volume_(volume), loader_(loader)
{
}

auto Script::Impl::Init(boost::filesystem::path const& file) -> void
{
  collect_ = event::Timer(step_time, -1);
  collect_.Add(function::Bind(&Impl::Collect, shared_from_this()));
  queue_.Add(function::Bind(&event::Timer::operator(), collect_));

  lua_.Resume();

  ActorInit();
  StageInit();
  ChoiceInit();
  SubtitleInit();
  TimerInit();
  ViewInit();
  CollisionInit();
  FadeInit();
  JointInit();
  AudioInit();

  typedef void(event::Signal::*Notify)();
  lua_.Add(function::Bind((Notify)&event::Signal::operator(), signal_), "exit", 0, "metallic_crow");

  lua_.Load(file);

  lua_.Collect(-1);
  lua_.Pause();
}

auto Script::Impl::Call(std::string const& call) -> void
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

auto Script::Impl::Collect() -> void
{
  lua_.Collect(step_size);
}

auto Script::Impl::Pause() -> void
{
  if(!paused_)
  {
    paused_ = true;
    for(auto& stage : stages_)
    {
      Pause(stage, stage->paused_[0]);
    }
    fade_.Pause();
    collect_.Pause();
  }
}

auto Script::Impl::Resume() -> void
{
  std::shared_ptr<Impl> guard = shared_from_this(); 

  if(!begun_)
  {
    begun_ = true;
    Call("main");
  }

  if(paused_)
  {
    paused_ = false;
    for(auto& stage : stages_)
    {
      Resume(stage, stage->paused_[0]);
    }
    fade_.Resume();
    collect_.Resume();
  }
}

auto Script::Impl::Render() -> void
{
  if(stage_)
  {
    View();
    for(auto actor : stage_->scene_.left)
    {
      actor.second.Render();
    }
    stage_->choice_.Render();
    stage_->subtitle_.Render();
  }
  fade_.Render();
}

auto Script::Impl::ChoiceUp() -> void
{
  if(stage_)
  {
    stage_->choice_.Up();
  }
}

auto Script::Impl::ChoiceDown() -> void
{
  if(stage_)
  {
    stage_->choice_.Down();
  }
}

auto Script::Impl::ChoiceLeft() -> void
{
  if(stage_)
  {
    stage_->choice_.Left();
  }
}

auto Script::Impl::ChoiceRight() -> void
{
  if(stage_)
  {
    stage_->choice_.Right();
  }
}

auto Script::Impl::Control(float x, float y) -> void
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

auto Script::Impl::Add(event::Command const& command) -> void
{
  signal_.Add(command);
}

auto Script::Pause() -> void
{
  impl_->Pause();
}

auto Script::Resume() -> void
{
  impl_->Resume();
}

auto Script::Render() -> void
{
  impl_->Render();
}

auto Script::ChoiceUp() -> void
{
  impl_->ChoiceUp();
}

auto Script::ChoiceDown() -> void
{
  impl_->ChoiceDown();
}

auto Script::ChoiceLeft() -> void
{
  impl_->ChoiceLeft();
}

auto Script::ChoiceRight() -> void
{
  impl_->ChoiceRight();
}

auto Script::Control(float x, float y) -> void
{
  impl_->Control(x, y);
}

auto Script::Add(event::Command const& command) -> void
{
  impl_->Add(command);
}

Script::operator bool() const
{
  return bool(impl_);
}

Script::Script(boost::filesystem::path const& file, display::Window& window, event::Queue& queue, boost::filesystem::path const& path, float volume, event::Timeslice& loader) : impl_(std::make_shared<Impl>(window, queue, path, volume, loader))
{
  impl_->Init(file);
}
}