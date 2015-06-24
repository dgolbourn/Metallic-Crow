#ifndef MIX_THREAD_H_
#define MIX_THREAD_H_
namespace mix
{
class Mutex
{
public:
  auto lock() const -> void;
  auto unlock() const noexcept -> void;
};

extern Mutex mutex;
}
#endif