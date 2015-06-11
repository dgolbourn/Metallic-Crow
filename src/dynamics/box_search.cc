#include "box_search.h"
namespace dynamics
{
BoxSearch::BoxSearch(b2AABB const& box, b2World const& world)
{
  world.QueryAABB(this, box);
}

bool BoxSearch::ReportFixture(b2Fixture* fixture)
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

BoxSearch::Iterator BoxSearch::begin()
{
  return impl_.begin();
}
  
BoxSearch::Iterator BoxSearch::end()
{
  return impl_.end();
}
}