#include "fade.h"
#include "timer.h"
#include "signal.h"
#include "bind.h"
namespace game
{
class Fade::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(float period, event::Queue& queue, display::Window const& window);
  void Down();
  void Up();
  void Render() const;
  void Pause();
  void Resume();
  void Next();
  void End();
  void Begin(bool up, int count, float begin, float end);
  void Period(float period);
  void Add(event::Command const& command);
  display::Window window_;
  display::Modulation modulation_;
  event::Timer timer_;
  event::Signal signal_;
  int count_;
  double period_;
  bool paused_;
  event::Queue queue_;
  bool up_;
};

Fade::Impl::Impl(float period, event::Queue& queue, display::Window const& window) : window_(window), modulation_(0.f, 0.f, 0.f, 1.f), count_(0), period_(period), paused_(true), queue_(queue), up_(false)
{
}

void Fade::Impl::Begin(bool up, int count, float begin, float end)
{
  if(period_ > 0.)
  {
    up_ = up;
    count_ = count;
    modulation_.a(begin);
    
    timer_ = event::Timer(period_ / 256., 255);
    timer_.Add(function::Bind(&Impl::Next, shared_from_this()));
    timer_.End(function::Bind(&Impl::End, shared_from_this()));
    queue_.Add(function::Bind(&event::Timer::operator(), timer_));
    if(!paused_)
    {
      timer_.Resume();
    }
  }
  else
  {
    modulation_.a(end);
  }
}

void Fade::Impl::Down()
{
  Begin(false, 256, 1.f, 0.f);
}

void Fade::Impl::Up()
{
  Begin(true, 0, 0.f, 1.f);
}

void Fade::Impl::Render() const
{
  window_.Draw(display::BoundingBox(), modulation_);
}

void Fade::Impl::Pause()
{
  if(!paused_)
  {
    paused_ = true;
    timer_.Pause();
  }
}

void Fade::Impl::Resume()
{
  if(paused_)
  {
    paused_ = false;
    timer_.Resume();
  }
}

void Fade::Impl::Next()
{
  if(up_)
  {
    ++count_;
  }
  else
  {
    --count_;
  }

  modulation_.a(count_ / 256.f);
}

void Fade::Impl::End()
{
  signal_();
}

void Fade::Impl::Add(event::Command const& command)
{
  signal_.Add(command);
}

void Fade::Impl::Period(float period)
{
  period_ = period;
}

Fade::Fade(float period, event::Queue& queue, display::Window const& window) : impl_(std::make_shared<Impl>(period, queue, window))
{
}

void Fade::Down()
{
  impl_->Down();
}

void Fade::Up()
{
  impl_->Up();
}

void Fade::Render() const
{
  impl_->Render();
}

void Fade::Pause()
{
  impl_->Pause();
}

void Fade::Resume()
{
  impl_->Resume();
}

void Fade::Add(event::Command const& command)
{
  impl_->Add(command);
}

void Fade::Period(float period)
{
  impl_->Period(period);
}
}