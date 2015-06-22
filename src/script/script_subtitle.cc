#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::SubtitleInit()
{
  lua_.Add(function::Bind(&Impl::SubtitleText, shared_from_this()), "subtitle_text", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::SubtitleModulation, shared_from_this()), "subtitle_modulation", 0, "metallic_crow");
}

void Script::Impl::SubtitleText()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-2);
    stage = StageGet();
  }
  if(stage)
  {
    std::string text;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(text);
    }
    stage->subtitle_(text);
  }
}

void Script::Impl::SubtitleModulation()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-5);
    stage = StageGet();
  }
  if(stage)
  {
    float r;
    {
      lua::Guard guard = lua_.Get(-4);
      lua_.Pop(r);
    }
    float g;
    {
      lua::Guard guard = lua_.Get(-3);
      lua_.Pop(g);
    }
    float b;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(b);
    }
    float a;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(a);
    }
    stage->subtitle_.Modulation(r, g, b, a);
  }
}
}