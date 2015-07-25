#include "script_impl.h"
#include "bind.h"
namespace game
{
auto Script::Impl::ChoiceInit() -> void
{
  lua_.Add(function::Bind(&Impl::ChoiceChoice, shared_from_this()), "choice", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ChoiceUpModulation, shared_from_this()), "choice_up_modulation", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ChoiceDownModulation, shared_from_this()), "choice_down_modulation", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ChoiceLeftModulation, shared_from_this()), "choice_left_modulation", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ChoiceRightModulation, shared_from_this()), "choice_right_modulation", 0, "metallic_crow");
}

auto Script::Impl::ChoiceChoice() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-6);
    stage = StageGet();
  }
  if(stage)
  {
    stage->choice_(lua_.At<std::string>(-5), lua_.At<std::string>(-4), lua_.At<std::string>(-3), lua_.At<std::string>(-2), lua_.At<double>(-1));
  }
}

auto Script::Impl::ChoiceUpModulation() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-5);
    stage = StageGet();
  }
  if(stage)
  {
    stage->choice_.Up(lua_.At<float>(-4), lua_.At<float>(-3), lua_.At<float>(-2), lua_.At<float>(-1));
  }
}

auto Script::Impl::ChoiceDownModulation() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-5);
    stage = StageGet();
  }
  if(stage)
  {
    stage->choice_.Down(lua_.At<float>(-4), lua_.At<float>(-3), lua_.At<float>(-2), lua_.At<float>(-1));
  }
}

auto Script::Impl::ChoiceLeftModulation() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-5);
    stage = StageGet();
  }
  if(stage)
  {
    stage->choice_.Left(lua_.At<float>(-4), lua_.At<float>(-3), lua_.At<float>(-2), lua_.At<float>(-1));
  }
}

auto Script::Impl::ChoiceRightModulation() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-5);
    stage = StageGet();
  }
  if(stage)
  {
    stage->choice_.Right(lua_.At<float>(-4), lua_.At<float>(-3), lua_.At<float>(-2), lua_.At<float>(-1));
  }
}
}