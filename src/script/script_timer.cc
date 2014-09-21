#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::TimerInit()
{
  lua_.Add(function::Bind(&Impl::TimerLoad, shared_from_this()), "timer_load", 0);
  lua_.Add(function::Bind(&Impl::TimerFree, shared_from_this()), "timer_free", 0);
}

void Script::Impl::TimerLoad()
{
  std::string call;
  lua_.PopFront(call);
  double interval;
  lua_.PopFront(interval);
  int loops;
  lua_.PopFront(loops);

  event::Timer timer(interval, loops);
  timer.Add(function::Bind(&Impl::Call, shared_from_this(), call));
  if(!Pause(stage_))
  {
    timer.Resume();
  }
  stage_->timers_.emplace(call, timer);
  queue_.Add(function::Bind(&event::Timer::operator(), timer));
}

void Script::Impl::TimerFree()
{
  std::string call;
  lua_.PopFront(call);
  stage_->timers_.erase(call);
}
}