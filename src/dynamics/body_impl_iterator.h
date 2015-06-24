#ifndef BODY_IMPL_ITERATOR_H_
#define BODY_IMPL_ITERATOR_H_
#include "body_impl.h"
#include "boost/iterator/iterator_facade.hpp"
namespace dynamics
{
class BodyImpl::Iterator final : public boost::iterator_facade<Iterator, BodyImpl, boost::forward_traversal_tag>
{ 
public:
  Iterator();
  explicit Iterator(BodyImpl* impl);
private:
  friend class boost::iterator_core_access;
  auto increment() -> void;
  auto equal(Iterator const& other) const -> bool;
  auto dereference() const -> BodyImpl&;
  BodyImpl* impl_;
};
}

auto begin(b2World const& world) -> dynamics::BodyImpl::Iterator;
auto end(b2World const& world) -> dynamics::BodyImpl::Iterator;
#endif