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
  Timer(void) = default;
  Timer(double interval, int loops);
  void Reset(double interval, int loops);
  void Pause(void);
  void Resume(void);
  void Add(Command const& command);
  void End(Command const& command);
  void operator()(void);
  explicit operator bool(void) const;
  typedef memory::WeakPtr<Timer, class TimerImpl> WeakPtr;
private:
  std::shared_ptr<class TimerImpl> impl_;
  friend WeakPtr;
};
}
#endif