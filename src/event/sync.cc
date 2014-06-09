#include "sync.h"
#include <chrono>
#include <thread>
namespace event
{
class SyncImpl
{
public:
  SyncImpl(double frame_rate);
  void Synchronise(void);
  std::chrono::high_resolution_clock::duration interval_;
  std::chrono::high_resolution_clock::time_point tick_;
};

SyncImpl::SyncImpl(double frame_rate)
{
  double scale = double(std::chrono::high_resolution_clock::period::den) / double(std::chrono::high_resolution_clock::period::num);
  double interval = scale / frame_rate;
  interval_ = std::chrono::high_resolution_clock::duration(long long(interval));
  tick_ = std::chrono::high_resolution_clock::now();
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