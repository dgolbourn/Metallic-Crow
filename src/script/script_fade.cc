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
  fade_.Up(lua_.At<float>(-1));
}

auto Script::Impl::FadeDown() -> void
{
  fade_.Down(lua_.At<float>(-1));
}

auto Script::Impl::FadeEnd() -> void
{
  fade_.Add(lua_.At<event::Command>(-1));
}
}