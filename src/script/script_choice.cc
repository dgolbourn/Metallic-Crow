#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::ChoiceInit()
{
  lua_.Add(function::Bind(&Impl::ChoiceChoice, shared_from_this()), "choice", 0);
  lua_.Add(function::Bind(&Impl::ChoiceUpModulation, shared_from_this()), "choice_up_modulation", 0);
  lua_.Add(function::Bind(&Impl::ChoiceDownModulation, shared_from_this()), "choice_down_modulation", 0);
  lua_.Add(function::Bind(&Impl::ChoiceLeftModulation, shared_from_this()), "choice_left_modulation", 0);
  lua_.Add(function::Bind(&Impl::ChoiceRightModulation, shared_from_this()), "choice_right_modulation", 0);
}

void Script::Impl::ChoiceChoice()
{
  StagePtr stage = StagePop();
  std::string up;
  lua_.PopFront(up);
  std::string down;
  lua_.PopFront(down);
  std::string left;
  lua_.PopFront(left);
  std::string right;
  lua_.PopFront(right);
  double interval;
  lua_.PopFront(interval);
  if(stage)
  {
    stage->choice_(up, down, left, right, interval);
  }
}

void Script::Impl::ChoiceUpModulation()
{
  StagePtr stage = StagePop();
  float r, g, b, a;
  lua_.PopFront(r);
  lua_.PopFront(g);
  lua_.PopFront(b);
  lua_.PopFront(a);
  if(stage)
  {
    stage->choice_.Up(r, g, b, a);
  }
}

void Script::Impl::ChoiceDownModulation()
{
  StagePtr stage = StagePop();
  float r, g, b, a;
  lua_.PopFront(r);
  lua_.PopFront(g);
  lua_.PopFront(b);
  lua_.PopFront(a);
  if(stage)
  {
    stage->choice_.Down(r, g, b, a);
  }
}

void Script::Impl::ChoiceLeftModulation()
{
  StagePtr stage = StagePop();
  float r, g, b, a;
  lua_.PopFront(r);
  lua_.PopFront(g);
  lua_.PopFront(b);
  lua_.PopFront(a);
  if(stage)
  {
    stage->choice_.Left(r, g, b, a);
  }
}

void Script::Impl::ChoiceRightModulation()
{
  StagePtr stage = StagePop();
  float r, g, b, a;
  lua_.PopFront(r);
  lua_.PopFront(g);
  lua_.PopFront(b);
  lua_.PopFront(a);
  if(stage)
  {
    stage->choice_.Right(r, g, b, a);
  }
}
}