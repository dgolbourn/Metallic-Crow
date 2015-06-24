#ifndef QUEUE_H_
#define QUEUE_H_
#include "command.h"
#include <memory>
namespace event
{
class Queue
{
public:
  Queue();
  auto Add(Command const& command) -> void;
  auto operator()() -> void;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif