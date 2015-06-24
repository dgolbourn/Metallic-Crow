#ifndef SYNC_H_
#define SYNC_H_
#include <memory>
namespace event
{
class Sync
{
public:
  Sync() = default;
  Sync(double frame_rate);
  auto operator()() -> void;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif