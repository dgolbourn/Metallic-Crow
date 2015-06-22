#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::ChoiceInit()
{
  lua_.Add(function::Bind(&Impl::ChoiceChoice, shared_from_this()), "choice", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ChoiceUpModulation, shared_from_this()), "choice_up_modulation", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ChoiceDownModulation, shared_from_this()), "choice_down_modulation", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ChoiceLeftModulation, shared_from_this()), "choice_left_modulation", 0, "metallic_crow");
  lua_.Add(function::Bind(&Impl::ChoiceRightModulation, shared_from_this()), "choice_right_modulation", 0, "metallic_crow");
}

void Script::Impl::ChoiceChoice()
{
  StagePtr stage;
  {
    lua::Guard guard = lua_.Get(-6);
    stage = StageGet();
  }
  if(stage)
  {
    std::string up;
    {
      lua::Guard guard = lua_.Get(-5);
      lua_.Pop(up);
    }
    std::string down;
    {
      lua::Guard guard = lua_.Get(-4);
      lua_.Pop(down);
    }
    std::string left;
    {
      lua::Guard guard = lua_.Get(-3);
      lua_.Pop(left);
    }
    std::string right;
    {
      lua::Guard guard = lua_.Get(-2);
      lua_.Pop(right);
    }
    double interval;
    {
      lua::Guard guard = lua_.Get(-1);
      lua_.Pop(interval);
    }
    stage->choice_(up, down, left, right, interval);
  }
}

void Script::Impl::ChoiceUpModulation()
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
    stage->choice_.Up(r, g, b, a);
  }
}

void Script::Impl::ChoiceDownModulation()
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
    stage->choice_.Down(r, g, b, a);
  }
}

void Script::Impl::ChoiceLeftModulation()
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
    stage->choice_.Left(r, g, b, a);
  }
}

void Script::Impl::ChoiceRightModulation()
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

    stage->choice_.Right(r, g, b, a);
  }
}
}