#include "queue.h"
#include <list>
#include "for_each.h"
namespace event
{
class Queue::Impl
{
public:
  auto Add(Command const& command) -> void;
  auto Check() -> void;
  std::list<Command> commands_;
};

auto Queue::Impl::Add(Command const& command) -> void
{
  commands_.push_back(command);
}

auto Queue::Impl::Check() -> void
{
  for_each(commands_);
}

auto Queue::Add(Command const& command) -> void
{
  impl_->Add(command);
}

auto Queue::operator()() -> void
{
  impl_->Check();
}

Queue::Queue() : impl_(std::make_shared<Impl>())
{
}
}