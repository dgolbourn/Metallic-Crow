#include "signal.h"
#include <list>
#include "bind.h"
namespace event
{
typedef std::list<Command> CommandList;

class SignalImpl final : public std::enable_shared_from_this<SignalImpl>
{
public:
  SignalImpl(void);
  void Notify(void);
  void Queue(Queue& queue);
  void Add(Command const& comand);
  bool Empty(void) const;
  void Clear(void);
  CommandList commands_;
  bool active_;
  bool clear_;
};

SignalImpl::SignalImpl(void) : active_(false), clear_(false)
{
}

void SignalImpl::Notify(void)
{
  if(!active_)
  {
    active_ = true;
    for(auto iter = commands_.begin(); iter != commands_.end();)
    {
      if(bool(*iter) && (*iter)())
      {
        ++iter;
      }
      else
      {
        iter = commands_.erase(iter);
      }
    }
    active_ = false;

    if(clear_)
    {
      clear_ = false;
      commands_.clear();
    }
  }
}

void SignalImpl::Queue(event::Queue& queue)
{
  auto command = event::Bind(&SignalImpl::Notify, shared_from_this());
  queue.Add([=](){command(); return false;});
}

void SignalImpl::Add(Command const& comand)
{
  commands_.push_back(comand);
}

bool SignalImpl::Empty(void) const
{
  return commands_.empty();
}

void SignalImpl::Clear(void)
{
  if(active_)
  {
    clear_ = true;
  }
  else
  {
    commands_.clear();
  }
}

void Signal::operator()(Queue& queue)
{
  impl_->Queue(queue);
}

void Signal::operator()(void)
{
  impl_->Notify();
}

void Signal::Add(Command const& comand)
{
  impl_->Add(comand);
}

Signal::Signal(void)
{
  impl_ = std::make_shared<SignalImpl>();
}

Signal::operator bool(void) const
{
  return !impl_->Empty();
}

void Signal::Clear(void)
{
  impl_->Clear();
}
}