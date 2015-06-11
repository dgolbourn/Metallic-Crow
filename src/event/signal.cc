#include "signal.h"
#include <list>
#include "bind.h"
namespace event
{
typedef std::list<Command> CommandList;

class Signal::Impl final : public std::enable_shared_from_this<Signal::Impl>
{
public:
  Signal::Impl();
  void Notify();
  void Queue(Queue& queue);
  void Add(Command const& comand);
  bool Empty() const;
  void Clear();
  CommandList commands_;
  bool active_;
  bool clear_;
};

Signal::Impl::Impl() : active_(false), clear_(false)
{
}

void Signal::Impl::Notify()
{
  if(!active_)
  {
    active_ = true;
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
    active_ = false;

    if(clear_)
    {
      clear_ = false;
      commands_.clear();
    }
  }
}

void Signal::Impl::Queue(event::Queue& queue)
{
  auto command = function::Bind(&Signal::Impl::Notify, shared_from_this());
  queue.Add([=](){command(); return false;});
}

void Signal::Impl::Add(Command const& comand)
{
  commands_.push_back(comand);
}

bool Signal::Impl::Empty() const
{
  return commands_.empty();
}

void Signal::Impl::Clear()
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

void Signal::operator()()
{
  impl_->Notify();
}

void Signal::Add(Command const& comand)
{
  impl_->Add(comand);
}

Signal::Signal() : impl_(std::make_shared<Impl>())
{
}

Signal::operator bool() const
{
  return !impl_->Empty();
}

void Signal::Clear()
{
  impl_->Clear();
}
}