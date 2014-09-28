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
  std::string text;
  lua_.PopFront(text);
  stage_->subtitle_(text);
}

void Script::Impl::SubtitleLight()
{
  float r, g, b;
  lua_.PopFront(r);
  lua_.PopFront(g);
  lua_.PopFront(b);
  stage_->subtitle_.Modulate(r, g, b);
}
}