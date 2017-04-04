#include "signal.h"
#include <list>
#include "bind.h"
#include "for_each.h"
namespace event
{
typedef std::list<Command> CommandList;

class Signal::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Signal::Impl();
  auto Notify() -> void;
  auto Queue(Queue& queue) -> void;
  auto Add(Command const& comand) -> void;
  auto Empty() const -> bool;
  auto Clear() -> void;
  CommandList commands_;
  bool active_;
  bool clear_;
};

Signal::Impl::Impl() : active_(false), clear_(false)
{
}

auto Signal::Impl::Notify() -> void
{
  if(!active_)
  {
    active_ = true;
    for_each(commands_);
    active_ = false;

    if(clear_)
    {
      clear_ = false;
      commands_.clear();
    }
  }
}

auto Signal::Impl::Queue(event::Queue& queue) -> void
{
  auto command = function::Bind(&Signal::Impl::Notify, shared_from_this());
  queue.Add([=](){command(); return false;});
}

auto Signal::Impl::Add(Command const& comand) -> void
{
  commands_.push_back(comand);
}

auto Signal::Impl::Empty() const -> bool
{
  return commands_.empty();
}

auto Signal::Impl::Clear() -> void
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

auto Signal::operator()(Queue& queue) -> void
{
  impl_->Queue(queue);
}

auto Signal::operator()() -> void
{
  impl_->Notify();
}

auto Signal::Add(Command const& comand) -> void
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

auto Signal::Clear() -> void
{
  impl_->Clear();
}
}