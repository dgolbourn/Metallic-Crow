#ifndef TIMER_H_
#define TIMER_H_
#include <memory>
#include "command.h"
#include "weak_ptr.h"
namespace event
{
class Timer
{
public:
  Timer() = default;
  Timer(double interval, int loops);
  auto Reset(double interval, int loops) -> void;
  auto Pause() -> void;
  auto Resume() -> void;
  auto Add(Command const& command) -> void;
  auto End(Command const& command) -> void;
  auto operator()() -> void;
  explicit operator bool() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
public:
  typedef memory::WeakPtr<Timer> WeakPtr;
  friend WeakPtr;
};
}
#endif