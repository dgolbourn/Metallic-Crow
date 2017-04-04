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
    lua::Guard guard = lua_.Get(-7);
    stage = StageGet();
  }
  if(stage)
  {
    auto iter = stage_->choices_.find(lua_.At<int>(-6));
    if(iter != stage_->choices_.end())
    {
      iter->second(lua_.At<std::string>(-5), lua_.At<std::string>(-4), lua_.At<std::string>(-3), lua_.At<std::string>(-2), lua_.At<double>(-1));
    }
  }
}

auto Script::Impl::ChoiceUpModulation() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-6);
    stage = StageGet();
  }
  if(stage)
  {    
    auto iter = stage_->choices_.find(lua_.At<int>(-5));
    if(iter != stage_->choices_.end())
    {
      iter->second.Up(lua_.At<float>(-4), lua_.At<float>(-3), lua_.At<float>(-2), lua_.At<float>(-1));
    }
  }
}

auto Script::Impl::ChoiceDownModulation() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-6);
    stage = StageGet();
  }
  if(stage)
  {
    auto iter = stage_->choices_.find(lua_.At<int>(-5));
    if(iter != stage_->choices_.end())
    {
      iter->second.Down(lua_.At<float>(-4), lua_.At<float>(-3), lua_.At<float>(-2), lua_.At<float>(-1));
    }
  }
}

auto Script::Impl::ChoiceLeftModulation() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-6);
    stage = StageGet();
  }
  if(stage)
  {
    auto iter = stage_->choices_.find(lua_.At<int>(-5));
    if(iter != stage_->choices_.end())
    {
      iter->second.Left(lua_.At<float>(-4), lua_.At<float>(-3), lua_.At<float>(-2), lua_.At<float>(-1));
    }
  }
}

auto Script::Impl::ChoiceRightModulation() -> void
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-6);
    stage = StageGet();
  }
  if(stage)
  {
    auto iter = stage_->choices_.find(lua_.At<int>(-5));
    if(iter != stage_->choices_.end())
    {
      iter->second.Right(lua_.At<float>(-4), lua_.At<float>(-3), lua_.At<float>(-2), lua_.At<float>(-1));
    }
  }
}
}