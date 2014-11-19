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
  StagePtr stage = StagePop();
  std::string name;
  lua_.PopFront(name);
  event::Command command;
  lua_.PopFront(command);
  double interval;
  lua_.PopFront(interval);
  int loops;
  lua_.PopFront(loops);

  if(stage)
  {
    event::Timer timer(interval, loops);
    timer.Add(command);
    if(!Pause(stage))
    {
      timer.Resume();
    }
    stage->timers_.emplace(name, timer);
    queue_.Add(function::Bind(&event::Timer::operator(), timer));
  }
}

void Script::Impl::TimerFree()
{
  StagePtr stage = StagePop();
  std::string name;
  lua_.PopFront(name);
  if(stage)
  {
    stage->timers_.erase(name);
  }
}
}