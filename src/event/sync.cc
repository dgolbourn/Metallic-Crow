#include "sync.h"
#include <chrono>
#include <thread>
namespace event
{
class SyncImpl
{
public:
  SyncImpl(float frame_rate);
  void Synchronise(void);
  std::chrono::milliseconds frame_period_;
  std::chrono::steady_clock::time_point tick_;
};

SyncImpl::SyncImpl(float frame_rate)
{
  frame_period_ = std::chrono::milliseconds(int(1000.f / frame_rate));
  std::chrono::steady_clock::time_point tick = std::chrono::steady_clock::now();
}

void SyncImpl::Synchronise(void)
{
  tick_ += frame_period_;
  std::this_thread::sleep_until(tick_);
}

Sync::Sync(float frame_rate)
{
  impl_ = std::make_shared<SyncImpl>(frame_rate);
}

void Sync::operator()(void)
{
  impl_->Synchronise();
}
}