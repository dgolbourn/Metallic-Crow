#include "queue.h"
#include <queue>
#include <mutex>
namespace event
{
class QueueImpl
{
public:
  void Add(Command const& command);
  void Check(void);
  std::mutex mutex_;
  std::queue<Command> queue_;
};

void QueueImpl::Add(Command const& command)
{
  queue_.push(command);
}

void QueueImpl::Check(void)
{
  while(!queue_.empty())
  {
    queue_.front()();
    queue_.pop(); 
  }
}

void Queue::Add(Command const& command)
{
  std::lock_guard<std::mutex> lock(impl_->mutex_);
  impl_->Add(command);
}

void Queue::operator()(void)
{
  std::lock_guard<std::mutex> lock(impl_->mutex_);
  impl_->Check();
}

Queue::Queue(void)
{
  impl_ = std::make_shared<QueueImpl>();
}
}