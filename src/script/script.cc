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
Script::Impl::Impl(display::Window& window, event::Queue& queue, boost::filesystem::path const& path, float volume) : window_(window), queue_(queue), paused_(true), fade_(queue, window), begun_(false), path_(path), lua_(path), volume_(volume)
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

auto Script::Impl::Player(int player, std::string const& call) -> void
{
  lua::Guard guard0 = lua_.Get("package");
  lua::Guard guard1 = lua_.Field("loaded");
  lua::Guard guard2 = lua_.Field("metallic_crow");
  lua::Guard guard3 = lua_.Field(call);
  if(lua_.Check())
  {
    lua::Guard guard4 = lua_.Push(player);
    lua_.Call(1, 0);
  }
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
    for(auto const& actor : stage_->scene_.left)
    {
      actor.second.Render();
    }
    for(auto& choice : stage_->choices_)
    {
      choice.second.Render();
    }
    stage_->subtitle_.Render();
    stage_->title_.Render();
  }
  fade_.Render();
}

auto Script::Impl::ChoiceUp(int player) -> void
{
  if(stage_)
  {
    auto iter = stage_->choices_.find(player);
    if(iter != stage_->choices_.end())
    {
      iter->second.Up();
    }
  }
}

auto Script::Impl::ChoiceDown(int player) -> void
{
  if(stage_)
  {
    auto iter = stage_->choices_.find(player);
    if(iter != stage_->choices_.end())
    {
      iter->second.Down();
    }
  }
}

auto Script::Impl::ChoiceLeft(int player) -> void
{
  if(stage_)
  {
    auto iter = stage_->choices_.find(player);
    if(iter != stage_->choices_.end())
    {
      iter->second.Left();
    }
  }
}

auto Script::Impl::ChoiceRight(int player) -> void
{
  if(stage_)
  {
    auto iter = stage_->choices_.find(player);
    if(iter != stage_->choices_.end())
    {
      iter->second.Right();
    }
  }
}

auto Script::Impl::ActionLeft(int player, bool state) -> void
{
  lua::Guard guard0 = lua_.Get("package");
  lua::Guard guard1 = lua_.Field("loaded");
  lua::Guard guard2 = lua_.Field("metallic_crow");
  lua::Guard guard3 = lua_.Field("action_left");
  if(lua_.Check())
  {
    lua::Guard guard4 = lua_.Push(player);
    lua::Guard guard5 = lua_.Push(state);
    lua_.Call(2, 0);
  }
}

auto Script::Impl::ActionRight(int player, bool state) -> void
{
  lua::Guard guard0 = lua_.Get("package");
  lua::Guard guard1 = lua_.Field("loaded");
  lua::Guard guard2 = lua_.Field("metallic_crow");
  lua::Guard guard3 = lua_.Field("action_right");
  if(lua_.Check())
  {
    lua::Guard guard4 = lua_.Push(player);
    lua::Guard guard5 = lua_.Push(state);
    lua_.Call(2, 0);
  }
}

auto Script::Impl::Move(int player, float x, float y) -> void
{
  lua::Guard guard0 = lua_.Get("package");
  lua::Guard guard1 = lua_.Field("loaded");
  lua::Guard guard2 = lua_.Field("metallic_crow");
  lua::Guard guard3 = lua_.Field("move");
  if(lua_.Check())
  {
    lua::Guard guard4 = lua_.Push(player);
    lua::Guard guard5 = lua_.Push(x);
    lua::Guard guard6 = lua_.Push(y);
    lua_.Call(3, 0);
  }
}

auto Script::Impl::Look(int player, float x, float y) -> void
{
  lua::Guard guard0 = lua_.Get("package");
  lua::Guard guard1 = lua_.Field("loaded");
  lua::Guard guard2 = lua_.Field("metallic_crow");
  lua::Guard guard3 = lua_.Field("look");
  if(lua_.Check())
  {
    lua::Guard guard4 = lua_.Push(player);
    lua::Guard guard5 = lua_.Push(x);
    lua::Guard guard6 = lua_.Push(y);
    lua_.Call(3, 0);
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

auto Script::ChoiceUp(int player) -> void
{
  impl_->ChoiceUp(player);
}

auto Script::ChoiceDown(int player) -> void
{
  impl_->ChoiceDown(player);
}

auto Script::ChoiceLeft(int player) -> void
{
  impl_->ChoiceLeft(player);
}

auto Script::ChoiceRight(int player) -> void
{
  impl_->ChoiceRight(player);
}

auto Script::ActionLeft(int player, bool state) -> void
{
  impl_->ActionLeft(player, state);
}

auto Script::ActionRight(int player, bool state) -> void
{
  impl_->ActionRight(player, state);
}

auto Script::Move(int player, float x, float y) -> void
{
  impl_->Move(player, x, y);
}

auto Script::Look(int player, float x, float y) -> void
{
  impl_->Look(player, x, y);
}

auto Script::Add(event::Command const& command) -> void
{
  impl_->Add(command);
}

Script::operator bool() const
{
  return static_cast<bool>(impl_);
}

Script::Script(boost::filesystem::path const& file, display::Window& window, event::Queue& queue, boost::filesystem::path const& path, float volume) : impl_(std::make_shared<Impl>(window, queue, path, volume))
{
  impl_->Init(file);
}
}