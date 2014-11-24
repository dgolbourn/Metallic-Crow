#include "fade.h"
#include "timer.h"
#include "signal.h"
#include "bind.h"
#include "colour.h"
namespace game
{
class Fade::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(event::Queue& queue, display::Window const& window);
  void Render() const;
  void Pause();
  void Resume();
  void Next();
  void Fade(double period, float end);
  void Add(event::Command const& command);
  display::Window window_;
  display::Modulation modulation_;
  event::Timer timer_;
  event::Signal signal_;
  int count_;
  float current_;
  bool paused_;
  event::Queue queue_;
  bool up_;
  int steps_;
};

Fade::Impl::Impl(event::Queue& queue, display::Window const& window) : window_(window), modulation_(0.f, 0.f, 0.f, 0.f), count_(0), current_(0.f), paused_(true), queue_(queue), up_(false), steps_(255)
{
}

void Fade::Impl::Fade(double period, float end)
{
  if(period > 0.)
  {
    steps_ = std::abs(sdl::Colour(current_) - sdl::Colour(end));

    if(steps_)
    {
      up_ = end > current_;
      if(up_)
      {
        count_ = 0;
      }
      else
      {
        count_ = steps_;
      }

      timer_ = event::Timer(period / steps_, steps_);
      timer_.Add(function::Bind(&Impl::Next, shared_from_this()));

      typedef void(event::Signal::*Notify)();
      timer_.End(function::Bind((Notify)&event::Signal::operator(), signal_));
      queue_.Add(function::Bind(&event::Timer::operator(), timer_));
      if(!paused_)
      {
        timer_.Resume();
      }
    }
  }
  else
  {
    modulation_.a(end);
  }
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
    if(timer_)
    {
      timer_.Pause();
    }
  }
}

void Fade::Impl::Resume()
{
  if(paused_)
  {
    paused_ = false;
    if(timer_)
    {
      timer_.Resume();
    }
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

  current_ = float(count_) / steps_;

  modulation_.a(current_);
}

void Fade::Impl::Add(event::Command const& command)
{
  signal_.Add(command);
}

Fade::Fade(event::Queue& queue, display::Window const& window) : impl_(std::make_shared<Impl>(queue, window))
{
}

void Fade::Down(float period)
{
  impl_->Fade(period, 0.f);
}

void Fade::Up(float period)
{
  impl_->Fade(period, 1.f);
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
}