#include "timer.h"
#include "signal.h"
#include <chrono>
namespace event
{
typedef std::chrono::high_resolution_clock Clock;

class Timer::Impl 
{
public:
  Impl(double interval, int loops);
  auto Pause() -> void;
  auto Resume() -> void;
  auto Reset(double interval, int loops) -> void;
  auto Add(Command const& command) -> void;
  auto End(Command const& command) -> void;
  auto Check() -> void;
  auto Valid() -> bool;

  Clock::duration interval_;
  Clock::time_point tick_;
  Clock::duration remaining_;
  Signal signal_;
  Signal end_;
  int loops_;
  bool paused_;
};

Timer::Impl::Impl(double interval, int loops) 
{
  Reset(interval, loops);

  remaining_ = interval_;
  paused_ = true;
}

auto Timer::Impl::Reset(double interval, int loops) -> void
{
  if(interval <= 0.)
  { 
    interval = 0.;
    loops = 0;
  }
  else if(loops >= 0)
  {
    ++loops;
  }
  
  static const double scale = static_cast<double>(Clock::period::den) / static_cast<double>(Clock::period::num);
  interval *= scale;
  interval_ = Clock::duration(Clock::rep(interval));
  loops_ = loops;
}

auto Timer::Impl::Pause() -> void
{
  if(!paused_)
  {
    remaining_ = interval_ + tick_ - Clock::now();
    paused_ = true;
  }
}

auto Timer::Impl::Resume() -> void
{
  if(paused_)
  {
    tick_ = Clock::now() - interval_ + remaining_;
    paused_ = false;
  }
}

auto Timer::Impl::Add(event::Command const& command) -> void
{
  signal_.Add(command);
}

auto Timer::Impl::End(event::Command const& command) -> void
{
  end_.Add(command);
}

auto Timer::Impl::Valid() -> bool
{
  return loops_ != 0;
}

auto Timer::Impl::Check() -> void
{
  if(!paused_ && (loops_ != 0))
  {
    Clock::time_point now = Clock::now();
    Clock::duration elapsed = now - tick_;

    while((loops_ != 0) && (elapsed >= interval_))
    {
      elapsed -= interval_;
      tick_ += interval_;
      signal_();
      if(loops_ > 0)
      {
        --loops_;
        if(loops_ == 0)
        {
          end_();
        }
      }
    }
  }
}

Timer::Timer(double interval, int loops) : impl_(std::make_shared<Impl>(interval, loops))
{
}

auto Timer::Pause() -> void
{
  impl_->Pause();
}

auto Timer::Resume() -> void
{
  impl_->Resume();
}

auto Timer::Add(event::Command const& command) -> void
{
  impl_->Add(command);
}

auto Timer::End(event::Command const& command) -> void
{
  impl_->End(command);
}

auto Timer::operator()() -> void
{
  impl_->Check();
}

auto Timer::Reset(double interval, int loops) -> void
{
  impl_->Reset(interval, loops);
}

Timer::operator bool() const
{
  return bool(impl_) && impl_->Valid();
}
}