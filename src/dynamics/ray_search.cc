#include "ray_search.h"
namespace dynamics
{
RaySearch::RaySearch(b2Vec2 const& point_a, b2Vec2 const& point_b, b2World const& world)
{
  world.RayCast(this, point_a, point_b);
}

float32 RaySearch::ReportFixture(b2Fixture* fixture, b2Vec2 const&, b2Vec2 const&, float32 fraction)
{
  if(b2Body* body = fixture->GetBody())
  {
    if(BodyImpl* impl = (BodyImpl*)body->GetUserData())
    {
      (void)impl_.insert(impl);
    }
  }
  return fraction;
}

RaySearch::Iterator RaySearch::begin(void)
{
  return impl_.begin();
}
  
RaySearch::Iterator RaySearch::end(void)
{
  return impl_.end();
}
}