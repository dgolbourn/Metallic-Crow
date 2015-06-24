#include "script_impl.h"
#include "bind.h"
namespace game
{
auto Script::Impl::FadeInit() -> void
{
  lua_.Add(function::Bind(&Impl::FadeUp, shared_from_this()), "fade_up", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::FadeDown, shared_from_this()), "fade_down", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::FadeEnd, shared_from_this()), "fade_end", 0, "metallic_crow");
}

auto Script::Impl::FadeUp() -> void
{
  float period;
  {
    lua::Guard guard = lua_.Get(-1);
    lua_.Pop(period);
  }
  fade_.Up(period);
}

auto Script::Impl::FadeDown() -> void
{
  float period;
  {
    lua::Guard guard = lua_.Get(-1);
    lua_.Pop(period);
  }
  fade_.Down(period);
}

auto Script::Impl::FadeEnd() -> void
{
  event::Command command;
  {
    lua::Guard guard = lua_.Get(-1);
    lua_.Pop(command);
  }
  fade_.Add(command);
}
}