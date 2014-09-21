#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::DialogueInit()
{
  lua_.Add(function::Bind(&Impl::DialogueChoice, shared_from_this()), "dialogue_choice", 0);
  lua_.Add(function::Bind(&Impl::DialogueText, shared_from_this()), "dialogue_text", 0);
}

void Script::Impl::DialogueChoice()
{
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
  stage_->dialogue_.Choice(up, down, left, right, interval);
}

void Script::Impl::DialogueText()
{
  std::string text;
  lua_.PopFront(text);
  stage_->dialogue_.Text(text);
}
}