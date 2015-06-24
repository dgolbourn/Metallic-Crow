#include "ray_search.h"
namespace dynamics
{
RaySearch::RaySearch(b2Vec2 const& point_a, b2Vec2 const& point_b, b2World const& world)
{
  world.RayCast(this, point_a, point_b);
}

auto RaySearch::ReportFixture(b2Fixture* fixture, b2Vec2 const&, b2Vec2 const&, float32 fraction) -> float32
{
  if(b2Body* body = fixture->GetBody())
  {
    if(BodyImpl* impl = static_cast<BodyImpl*>(body->GetUserData()))
    {
      impl_.insert(impl);
    }
  }
  return fraction;
}

auto RaySearch::begin() -> Iterator
{
  return impl_.begin();
}
  
auto RaySearch::end() -> Iterator
{
  return impl_.end();
}
}