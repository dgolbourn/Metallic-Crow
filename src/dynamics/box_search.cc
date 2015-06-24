#include "box_search.h"
namespace dynamics
{
BoxSearch::BoxSearch(b2AABB const& box, b2World const& world)
{
  world.QueryAABB(this, box);
}

auto BoxSearch::ReportFixture(b2Fixture* fixture) -> bool
{
  if(b2Body* body = fixture->GetBody())
  {
    if(BodyImpl* impl = (BodyImpl*)body->GetUserData())
    {
      impl_.insert(impl);
    }
  }
  return true;
}

auto BoxSearch::begin() -> BoxSearch::Iterator
{
  return impl_.begin();
}
  
auto BoxSearch::end() -> BoxSearch::Iterator
{
  return impl_.end();
}
}