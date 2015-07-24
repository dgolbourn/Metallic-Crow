#include "script_impl.h"
#include "bind.h"
namespace game
{
auto Script::Impl::TimerInit() -> void
{
  lua::Init<std::pair<WeakStagePtr, event::Timer::WeakPtr>>(static_cast<lua_State*>(lua_));
  lua_.Add(function::Bind(&Impl::TimerLoad, shared_from_this()), "timer_load", 1, "metallic_crow");
  lua_.Add(function::Bind(&Impl::TimerFree, shared_from_this()), "timer_free", 0, "metallic_crow");
}

auto Script::Impl::TimerLoad() -> void
{
  event::Timer timer;

  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-4);
    stage = StageGet();
  }
  if(stage)
  {
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

    timer = event::Timer(interval, loops);
    timer.Add(command);
    if(!Pause(stage))
    {
      timer.Resume();
    }
    stage->timers_.emplace(timer);
    queue_.Add(function::Bind(&event::Timer::operator(), timer));
  }

  lua::Push(static_cast<lua_State*>(lua_), std::pair<WeakStagePtr, event::Timer::WeakPtr>(stage, timer));
}

auto Script::Impl::TimerFree() -> void
{
  std::pair<StagePtr, event::Timer> timer = StageDataGet<event::Timer>(); 
  if(timer.first && timer.second)
  {
    timer.first->timers_.erase(timer.second);
  }
}
}