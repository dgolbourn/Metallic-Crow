#ifndef SIGNAL_H_
#define SIGNAL_H_
#include <memory>
#include "command.h"
#include "queue.h"
#include "weak_ptr.h"
namespace event
{
class Signal
{
public:
  Signal(void);
  void operator()(void);
  void operator()(Queue& queue);
  void Add(Command const& command);
  void Clear(void);
  explicit operator bool(void) const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
public:
  typedef memory::WeakPtr<Signal> WeakPtr;
  friend WeakPtr;
};
}
#endif