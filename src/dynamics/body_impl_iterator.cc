#include "body_impl_iterator.h"
namespace dynamics
{
BodyImpl::Iterator::Iterator() : impl_(nullptr)
{
}

BodyImpl::Iterator::Iterator(BodyImpl* impl) : impl_(impl)
{
}

auto BodyImpl::Iterator::increment() -> void
{
  if(impl_)
  {
    b2Body* body = impl_->body_;
    if(body)
    {
      body = body->GetNext();
      if(body)
      {
        impl_ = static_cast<BodyImpl*>(body->GetUserData());
      }
      else
      {
        impl_ = nullptr;
      }
    }
  }
}

auto BodyImpl::Iterator::equal(Iterator const& other) const -> bool
{
  return impl_ == other.impl_;
}

auto BodyImpl::Iterator::dereference() const -> BodyImpl&
{ 
  return *impl_; 
}
}

auto begin(b2World const& world) -> dynamics::BodyImpl::Iterator
{
  dynamics::BodyImpl* impl = nullptr;
  if(b2Body const* body = world.GetBodyList())
  {
    impl = (dynamics::BodyImpl*)body->GetUserData();
  }
  return dynamics::BodyImpl::Iterator(impl);
}

auto end(b2World const&) -> dynamics::BodyImpl::Iterator
{
  return dynamics::BodyImpl::Iterator();
}