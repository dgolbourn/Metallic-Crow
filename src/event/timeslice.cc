#include "timeslice.h"
#include <list>
#include <chrono>
#include <cmath>
namespace
{
typedef std::chrono::high_resolution_clock Clock;

double Count(double time)
{
  double const scale = static_cast<double>(Clock::period::num) / static_cast<double>(Clock::period::den);
  return time * scale;
}
}

namespace event
{
class Timeslice::Impl
{
public:
  Impl(lua::Stack& lua);
  auto Add(Command const& command) -> void;
  auto Resume() -> void;
  auto Pause() -> void;
  auto Check() -> void;
  auto Tick(double rate) -> void;

  std::list<Command> commands_;
  std::list<Command>::iterator iterator_;

  bool paused_;
  Clock::duration pause_;

  double capacity_;
  double remaining_;
  Clock::time_point tick_;
  double rate_;
};

Timeslice::Impl::Impl(lua::Stack& lua) : paused_(true), rate_(lua.Field<double>("rate")), capacity_(Count(lua.Field<double>("capacity"))), remaining_(capacity_)
{
}

auto Timeslice::Impl::Add(Command const& command) -> void
{
  if(commands_.empty())
  {
    commands_.push_back(command);
    iterator_ = commands_.begin();
  }
  else
  {
    commands_.push_back(command);
  }
}

auto Timeslice::Impl::Tick(double rate) -> void
{
  auto now = Clock::now();
  remaining_ = std::fmin(remaining_ + (now - tick_).count() * rate, capacity_);
  tick_ = now;
}

auto Timeslice::Impl::Check() -> void
{
  if(!paused_ && !commands_.empty())
  {
    Tick(rate_);
  
    while(remaining_ > 0.)
    {
      if((*iterator_)())
      {
        ++iterator_;
      }
      else
      {
        iterator_ = commands_.erase(iterator_);
        if(commands_.empty())
        {
          Tick(rate_ - 1.);
          break;
        }
      }

      if(iterator_ == commands_.end())
      {  
        iterator_ = commands_.begin();
      }

      Tick(rate_ - 1.);
    }
  }
}

auto Timeslice::Impl::Pause() -> void
{
  if(!paused_)
  {
    pause_ = Clock::now() - tick_;
    paused_ = true;
  }
}

auto Timeslice::Impl::Resume() -> void
{
  if(paused_)
  {
    tick_ = Clock::now() - pause_;
    paused_ = false;
  }
}

auto Timeslice::Add(Command const& command) -> void
{
  impl_->Add(command);
}

auto Timeslice::operator()() -> void
{
  impl_->Check();
}

auto Timeslice::Pause() -> void
{
  impl_->Pause();
}

auto Timeslice::Resume() -> void
{
  impl_->Resume();
}

Timeslice::Timeslice(lua::Stack& lua) : impl_(std::make_shared<Impl>(lua))
{
}

Timeslice::operator bool() const
{
  return static_cast<bool>(impl_);
}
}