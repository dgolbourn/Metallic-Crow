#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::SubtitleInit()
{
  lua_.Add(function::Bind(&Impl::SubtitleText, shared_from_this()), "subtitle_text", 0);
  lua_.Add(function::Bind(&Impl::SubtitleLight, shared_from_this()), "subtitle_light", 0);
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

void Script::Impl::SubtitleLight()
{
  StagePtr stage = StagePop();
  float r, g, b;
  lua_.PopFront(r);
  lua_.PopFront(g);
  lua_.PopFront(b);
  if(stage)
  { 
    stage->subtitle_.Modulate(r, g, b);
  }
}
}