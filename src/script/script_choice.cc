#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::ChoiceInit()
{
  lua_.Add(function::Bind(&Impl::ChoiceChoice, shared_from_this()), "choice", 0);
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
}