#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::FadeInit()
{
  lua_.Add(function::Bind(&Impl::FadeUp, shared_from_this()), "fade_up", 0);
  lua_.Add(function::Bind(&Impl::FadeDown, shared_from_this()), "fade_down", 0);
}

void Script::Impl::FadeUp()
{
  float period;
  lua_.PopFront(period);
  fade_.Up(period);
}

void Script::Impl::FadeDown()
{
  float period;
  lua_.PopFront(period);
  fade_.Down(period);
}
}