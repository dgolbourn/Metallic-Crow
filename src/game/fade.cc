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
  auto Render() const -> void;
  auto Pause() -> void;
  auto Resume() -> void;
  auto Next() -> void;
  auto Fade(double period, float end) -> void;
  auto Add(event::Command const& command) -> void;
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

auto Fade::Impl::Fade(double period, float end) -> void
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

auto Fade::Impl::Render() const -> void
{
  window_.Draw(display::BoundingBox(), modulation_);
}

auto Fade::Impl::Pause() -> void
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

auto Fade::Impl::Resume() -> void
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

auto Fade::Impl::Next() -> void
{
  if(up_)
  {
    ++count_;
  }
  else
  {
    --count_;
  }

  current_ = static_cast<float>(count_) / steps_;

  modulation_.a(current_);
}

auto Fade::Impl::Add(event::Command const& command) -> void
{
  signal_.Add(command);
}

Fade::Fade(event::Queue& queue, display::Window const& window) : impl_(std::make_shared<Impl>(queue, window))
{
}

auto Fade::Down(float period) -> void
{
  impl_->Fade(period, 0.f);
}

auto Fade::Up(float period) -> void
{
  impl_->Fade(period, 1.f);
}

auto Fade::Render() const -> void
{
  impl_->Render();
}

auto Fade::Pause() -> void
{
  impl_->Pause();
}

auto Fade::Resume() -> void
{
  impl_->Resume();
}

auto Fade::Add(event::Command const& command) -> void
{
  impl_->Add(command);
}
}