#include "sync.h"
#include <chrono>
#include <thread>
namespace event
{
typedef std::chrono::high_resolution_clock Clock;

class SyncImpl
{
public:
  SyncImpl(double frame_rate);
  void Synchronise(void);
  Clock::duration interval_;
  Clock::time_point tick_;
};

SyncImpl::SyncImpl(double frame_rate)
{
  static const double scale = double(Clock::period::den) / double(Clock::period::num);
  double interval = scale / frame_rate;
  interval_ = Clock::duration(Clock::rep(interval));
  tick_ = Clock::now();
}

void SyncImpl::Synchronise(void)
{
  tick_ += interval_;
  std::this_thread::sleep_until(tick_);
}

Sync::Sync(double frame_rate)
{
  impl_ = std::make_shared<SyncImpl>(frame_rate);
}

void Sync::operator()(void)
{
  impl_->Synchronise();
}
}