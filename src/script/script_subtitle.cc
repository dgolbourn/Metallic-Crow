#include "script_impl.h"
#include "bind.h"
namespace game
{
auto Script::Impl::SubtitleInit() -> void
{
  lua_.Add(function::Bind(&Impl::SubtitleText, shared_from_this()), "subtitle_text", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::SubtitleModulation, shared_from_this()), "subtitle_modulation", 0, "metallic_crow");
}

auto Script::Impl::SubtitleText() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-2);
    stage = StageGet();
  }
  if(stage)
  {
    stage->subtitle_(lua_.At<std::string>(-1));
  }
}

auto Script::Impl::SubtitleModulation() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-5);
    stage = StageGet();
  }
  if(stage)
  {
    stage->subtitle_.Modulation(lua_.At<float>(-4), lua_.At<float>(-3), lua_.At<float>(-2), lua_.At<float>(-1));
  }
}
}