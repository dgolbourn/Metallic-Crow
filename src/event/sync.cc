#include "sync.h"
#include <chrono>
#include <thread>
namespace 
{
typedef std::chrono::high_resolution_clock Clock;
}

namespace event
{
class Sync::Impl
{
public:
  Impl(double frame_rate);
  auto Synchronise() -> void;
  Clock::duration interval_;
  Clock::time_point tick_;
};

Sync::Impl::Impl(double frame_rate)
{
  static const double scale = static_cast<double>(Clock::period::den) / static_cast<double>(Clock::period::num);
  double interval = scale / frame_rate;
  interval_ = static_cast<Clock::duration>(static_cast<Clock::rep>(interval));
  tick_ = Clock::now();
}

auto Sync::Impl::Synchronise() -> void
{
  tick_ += interval_;
  std::this_thread::sleep_until(tick_);
}

Sync::Sync(double frame_rate) : impl_(std::make_shared<Impl>(frame_rate))
{
}

auto Sync::operator()() -> void
{
  impl_->Synchronise();
}
}