#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::SubtitleInit()
{
  lua_.Add(function::Bind(&Impl::SubtitleText, shared_from_this()), "subtitle_text", 0);
  lua_.Add(function::Bind(&Impl::SubtitleModulation, shared_from_this()), "subtitle_modulation", 0);
}

void Script::Impl::SubtitleText()
{
  StagePtr stage = StagePop();
  std::string text;
  lua_.PopFront(text);
  if(stage)
  {
    stage->subtitle_(text);
  }
}

void Script::Impl::SubtitleModulation()
{
  StagePtr stage = StagePop();
  float r, g, b, a;
  lua_.PopFront(r);
  lua_.PopFront(g);
  lua_.PopFront(b);
  lua_.PopFront(a);
  if(stage)
  { 
    stage->subtitle_.Modulation(r, g, b, a);
  }
}
}