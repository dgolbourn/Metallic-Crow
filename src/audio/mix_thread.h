#ifndef MIX_THREAD_H_
#define MIX_THREAD_H_
namespace mix
{
class Mutex
{
public:
  void lock() const;
  void unlock() const noexcept;
};

extern Mutex mutex;
}
#endif