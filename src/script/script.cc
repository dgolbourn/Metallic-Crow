#include "script_impl.h"
#include "bind.h"
#include "exception.h"
namespace game
{
Script::Impl::Impl(display::Window& window, event::Queue& queue, boost::filesystem::path const& path) : window_(window), queue_(queue), paused_(true), fade_(queue, window), begun_(false), path_(path)
{
}

void Script::Impl::Init(boost::filesystem::path const& file)
{
  lua_.Load(file);

  ActorInit();
  StageInit();
  SetInit();
  ScreenInit();
  ChoiceInit();
  SubtitleInit();
  TimerInit();
  ViewInit();
  CollisionInit();
  FadeInit();

  typedef void (event::Signal::*Notify)();
  lua_.Add(function::Bind((Notify)&event::Signal::operator(), signal_), "quit", 0);

  Call("initialise");
}

void Script::Impl::Call(std::string const& call)
{
  lua_.Get(call);
  lua_.Call(0, 0);
}

void Script::Impl::Pause(void)
{
  if(!paused_)
  {
    paused_ = true;
    for(auto& stage : stages_)
    {
      Pause(stage.second, stage.second->paused_[0]);
    }
  }
}

void Script::Impl::Resume()
{
  if(!begun_)
  {
    begun_ = true;
    Call("begin");
  }

  if(paused_)
  {
    paused_ = false;
    for(auto& stage : stages_)
    {
      Resume(stage.second, stage.second->paused_[0]);
    }
  }
}

void Script::Impl::Render(void)
{
  if(stage_)
  {
    stage_->scene_.Render();
    stage_->choice_.Render();
    stage_->subtitle_.Render();
  }
  fade_.Render();
}

void Script::Impl::ChoiceUp(void)
{
  stage_->choice_.Up();
}

void Script::Impl::ChoiceDown(void)
{
  stage_->choice_.Down();
}

void Script::Impl::ChoiceLeft(void)
{
  stage_->choice_.Left();
}

void Script::Impl::ChoiceRight(void)
{
  stage_->choice_.Right();
}

void Script::Impl::Up(void)
{
  if(stage_->hero_)
  {
    stage_->hero_.Up();
  }
}

void Script::Impl::Down(void)
{
  if(stage_->hero_)
  {
    stage_->hero_.Down();
  }
}

void Script::Impl::Left(void)
{
  if(stage_->hero_)
  {
    stage_->hero_.Left();
  }
}

void Script::Impl::Right(void)
{
  if(stage_->hero_)
  {
    stage_->hero_.Right();
  }
}

void Script::Impl::Add(event::Command const& command)
{
  signal_.Add(command);
}

void Script::Pause(void)
{
  impl_->Pause();
}

void Script::Resume(void)
{
  impl_->Resume();
}

void Script::Render(void)
{
  impl_->Render();
}

void Script::ChoiceUp(void)
{
  impl_->ChoiceUp();
}

void Script::ChoiceDown(void)
{
  impl_->ChoiceDown();
}

void Script::ChoiceLeft(void)
{
  impl_->ChoiceLeft();
}

void Script::ChoiceRight(void)
{
  impl_->ChoiceRight();
}

void Script::Up(void)
{
  impl_->Up();
}

void Script::Down(void)
{
  impl_->Down();
}

void Script::Left(void)
{
  impl_->Left();
}

void Script::Right(void)
{
  impl_->Right();
}

void Script::Add(event::Command const& command)
{
  impl_->Add(command);
}

Script::operator bool(void) const
{
  return bool(impl_);
}

Script::Script(boost::filesystem::path const& file, display::Window& window, event::Queue& queue, boost::filesystem::path const& path) : impl_(std::make_shared<Impl>(window, queue, path))
{
  impl_->Init(file);
}
}