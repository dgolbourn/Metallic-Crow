#ifndef TIMER_H_
#define TIMER_H_
#include <memory>
#include <vector>
#include "signal.h"

namespace event
{
class Timer
{
public:
  Timer(void);
  Timer(int interval, bool repeats);
  ~Timer(void);
  Timer::Timer(Timer const& other);
  Timer(Timer&& other);
  Timer& operator=(Timer other);

  void Pause(void);
  void Resume(void);
  void Restart(void);
  Signal Signal(void);
private:
  std::shared_ptr<class TimerImpl> impl_;
};
}
#endif