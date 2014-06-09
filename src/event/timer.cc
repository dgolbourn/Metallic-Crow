#include "timer.h"
#include "signal.h"
#include <chrono>
namespace event
{
class TimerImpl 
{
public:
  TimerImpl(double interval, int loops);
  void Pause(void);
  void Resume(void);
  void Reset(double interval, int loops);
  void Add(Command const& command);
  void End(Command const& command);
  void Check(void);
  bool Valid(void);

  std::chrono::high_resolution_clock::duration interval_;
  std::chrono::high_resolution_clock::time_point tick_;
  std::chrono::high_resolution_clock::duration remaining_;
  Signal signal_;
  Signal end_;
  int loops_;
  bool paused_;
};

TimerImpl::TimerImpl(double interval, int loops) 
{
  Reset(interval, loops);
}

void TimerImpl::Reset(double interval, int loops)
{
  double scale = double(std::chrono::high_resolution_clock::period::den) / double(std::chrono::high_resolution_clock::period::num);
  interval *= scale;
  interval_ = std::chrono::high_resolution_clock::duration(long long(interval));
  remaining_ = interval_;
  paused_ = true;
  if(loops >= 0)
  {
    ++loops;
  }
  loops_ = loops;
}

void TimerImpl::Pause(void)
{
  if(!paused_)
  {
    remaining_ = interval_ + tick_ - std::chrono::high_resolution_clock::now();
    paused_ = true;
  }
}

void TimerImpl::Resume(void)
{
  if(paused_)
  {
    tick_ = std::chrono::high_resolution_clock::now() - interval_ + remaining_;
    paused_ = false;
  }
}

void TimerImpl::Add(event::Command const& command)
{
  signal_.Add(command);
}

void TimerImpl::End(event::Command const& command)
{
  end_.Add(command);
}

bool TimerImpl::Valid(void)
{
  return loops_ != 0;
}

void TimerImpl::Check(void)
{
  if(!paused_ && (loops_ != 0))
  {
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::duration elapsed = now - tick_;

    while((loops_ != 0) && (elapsed >= interval_))
    {
      elapsed -= interval_;
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
    tick_ = now - elapsed;
  }
}

Timer::Timer(double interval, int loops)
{
  impl_ = std::make_shared<TimerImpl>(interval, loops);
}

void Timer::Pause(void)
{
  impl_->Pause();
}

void Timer::Resume(void)
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

void Timer::operator()(void)
{
  impl_->Check();
}

void Timer::Reset(double interval, int loops)
{
  impl_->Reset(interval, loops);
}

Timer::operator bool(void) const
{
  return bool(impl_) && impl_->Valid();
}
}