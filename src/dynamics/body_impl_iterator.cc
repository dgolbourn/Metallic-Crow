#include "body_impl_iterator.h"
namespace dynamics
{
BodyImpl::Iterator::Iterator(void) : impl_(nullptr)
{
}

BodyImpl::Iterator::Iterator(BodyImpl* impl) : impl_(impl)
{
}

void BodyImpl::Iterator::increment(void)
{
  if(impl_)
  {
    if(b2Body* body = impl_->body_)
    {
      if(body = body->GetNext())
      {
        impl_ = (BodyImpl*)body->GetUserData();
      }
      else
      {
        impl_ = nullptr;
      }
    }
  }
}

bool BodyImpl::Iterator::equal(Iterator const& other) const
{
  return impl_ == other.impl_;
}

BodyImpl& BodyImpl::Iterator::dereference(void) const
{ 
  return *impl_; 
}
}

dynamics::BodyImpl::Iterator begin(b2World const& world)
{
  dynamics::BodyImpl* impl = nullptr;
  if(b2Body const* body = world.GetBodyList())
  {
    impl = (dynamics::BodyImpl*)body->GetUserData();
  }
  return dynamics::BodyImpl::Iterator(impl);
}

dynamics::BodyImpl::Iterator end(b2World const&)
{
  return dynamics::BodyImpl::Iterator();
}