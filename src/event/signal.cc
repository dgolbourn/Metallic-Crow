#include "signal.h"
#include <list>
#include "bind.h"
namespace event
{
typedef std::list<Command> CommandList;

class SignalImpl final : public std::enable_shared_from_this<SignalImpl>
{
public:
  void Notify(void);
  void Queue(Queue& queue);
  void Add(Command const& comand);
  bool Empty(void) const;
  void Clear(void);
  CommandList commands_;
};

void SignalImpl::Notify(void)
{
  for(auto iter = commands_.begin(); iter != commands_.end();)
  {
    if((*iter)())
    {
      ++iter;
    }
    else
    {
      iter = commands_.erase(iter);
    }
  }
}

void SignalImpl::Queue(event::Queue& queue)
{
  queue.Add(event::Bind(&SignalImpl::Notify, shared_from_this()));
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
  commands_.clear();
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