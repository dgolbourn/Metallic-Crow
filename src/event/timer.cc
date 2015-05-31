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
  void Pause();
  void Resume();
  void Reset(double interval, int loops);
  void Add(Command const& command);
  void End(Command const& command);
  void Check();
  bool Valid();

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

void Timer::Impl::Reset(double interval, int loops)
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
  
  static const double scale = double(Clock::period::den) / double(Clock::period::num);
  interval *= scale;
  interval_ = Clock::duration(Clock::rep(interval));
  loops_ = loops;
}

void Timer::Impl::Pause()
{
  if(!paused_)
  {
    remaining_ = interval_ + tick_ - Clock::now();
    paused_ = true;
  }
}

void Timer::Impl::Resume()
{
  if(paused_)
  {
    tick_ = Clock::now() - interval_ + remaining_;
    paused_ = false;
  }
}

void Timer::Impl::Add(event::Command const& command)
{
  signal_.Add(command);
}

void Timer::Impl::End(event::Command const& command)
{
  end_.Add(command);
}

bool Timer::Impl::Valid()
{
  return loops_ != 0;
}

void Timer::Impl::Check()
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

void Timer::Pause()
{
  impl_->Pause();
}

void Timer::Resume()
{
  impl_->Resume();
}

void Timer::Add(event::Command const& command)
{
  impl_->Add(command);
}

void Timer::End(event::Command const& command)
{
  impl_->End(command);
}

void Timer::operator()()
{
  impl_->Check();
}

void Timer::Reset(double interval, int loops)
{
  impl_->Reset(interval, loops);
}

Timer::operator bool() const
{
  return bool(impl_) && impl_->Valid();
}
}