#ifndef BODY_IMPL_ITERATOR_H_
#define BODY_IMPL_ITERATOR_H_
#include "body_impl.h"
#include "boost/iterator/iterator_facade.hpp"
namespace dynamics
{
class BodyImpl::Iterator : public boost::iterator_facade<Iterator, BodyImpl, boost::forward_traversal_tag>
{ 
public:
  Iterator();
  explicit Iterator(BodyImpl* impl);
private:
  friend class boost::iterator_core_access;
  void increment();
  bool equal(Iterator const& other) const;
  BodyImpl& dereference() const;
  BodyImpl* impl_;
};
}

dynamics::BodyImpl::Iterator begin(b2World const& world);
dynamics::BodyImpl::Iterator end(b2World const& world);
#endif