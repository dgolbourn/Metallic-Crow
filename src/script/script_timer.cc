#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::TimerInit()
{
  lua_.Add(function::Bind(&Impl::TimerLoad, shared_from_this()), "timer_load", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::TimerFree, shared_from_this()), "timer_free", 0, "metallic_crow");
}

void Script::Impl::TimerLoad()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-5);
    stage = StageGet();
  }
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-4);
      lua_.Pop(name);
    }

    event::Command command;
    {
      lua::Guard guard = lua_.Get(-3);
      lua_.Pop(command);
    }

    double interval;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(interval);
    }

    int loops;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(loops);
    }

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
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-2);
    stage = StageGet();
  }
  if(stage)
  {
    std::string name;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(name);
    }
    stage->timers_.erase(name);
  }
}
}