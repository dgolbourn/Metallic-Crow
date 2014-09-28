#ifndef SYNC_H_
#define SYNC_H_
#include <memory>
namespace event
{
class Sync
{
public:
  Sync(void) = default;
  Sync(double frame_rate);
  void operator()(void);
private:
  std::shared_ptr<class SyncImpl> impl_;
};
}
#endif