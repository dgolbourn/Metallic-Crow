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
  void operator()();
private:
  std::shared_ptr<class SyncImpl> impl_;
};
}
#endif