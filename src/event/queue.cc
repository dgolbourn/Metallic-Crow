#include "queue.h"
#include <list>
namespace event
{
class QueueImpl
{
public:
  void Add(Command const& command);
  void Check();
  std::list<Command> commands_;
};

void QueueImpl::Add(Command const& command)
{
  commands_.push_back(command);
}

void QueueImpl::Check()
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

void Queue::Add(Command const& command)
{
  impl_->Add(command);
}

void Queue::operator()()
{
  impl_->Check();
}

Queue::Queue()
{
  impl_ = std::make_shared<QueueImpl>();
}
}