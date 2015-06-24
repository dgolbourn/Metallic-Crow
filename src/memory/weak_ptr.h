#ifndef WEAK_PTR_H_
#define WEAK_PTR_H_
#include <memory>
namespace memory
{
template<class T, class TImpl = T::Impl> class WeakPtr
{
  std::weak_ptr<TImpl> impl_;
public:
  WeakPtr() = default;

  WeakPtr(T const& object) : impl_(object.impl_)
  {
  }

  auto Lock() const -> T
  {
    T object;
    object.impl_ = impl_.lock();
    return object;
  }

  auto operator<(WeakPtr const& other) const -> bool
  {
    return impl_.owner_before(other.impl_);
  }
};
}
#endif