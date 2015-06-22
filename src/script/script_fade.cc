#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::FadeInit()
{
  lua_.Add(function::Bind(&Impl::FadeUp, shared_from_this()), "fade_up", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::FadeDown, shared_from_this()), "fade_down", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::FadeEnd, shared_from_this()), "fade_end", 0, "metallic_crow");
}

void Script::Impl::FadeUp()
{
  float period;
  {
    lua::Guard guard = lua_.Get(-1);
    lua_.Pop(period);
  }
  fade_.Up(period);
}

void Script::Impl::FadeDown()
{
  float period;
  {
    lua::Guard guard = lua_.Get(-1);
    lua_.Pop(period);
  }
  fade_.Down(period);
}

void Script::Impl::FadeEnd()
{
  event::Command command;
  {
    lua::Guard guard = lua_.Get(-1);
    lua_.Pop(command);
  }
  fade_.Add(command);
}
}