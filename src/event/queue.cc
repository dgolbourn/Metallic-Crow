#include "queue.h"
#include <list>
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

auto Queue::Add(Command const& command) -> void
{
  impl_->Add(command);
}

auto Queue::operator()() -> void
{
  impl_->Check();
}

Queue::Queue()
{
  impl_ = std::make_shared<Impl>();
}
}