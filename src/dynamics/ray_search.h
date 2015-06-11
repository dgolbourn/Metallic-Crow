#ifndef RAY_SEARCH_H_
#define RAY_SEARCH_H_
#include "Box2D/Box2D.h"
#include "body_impl.h"
#include <set>
namespace dynamics
{
class RaySearch final : public b2RayCastCallback
{
  typedef std::set<BodyImpl*> BodySet;
public:
  RaySearch(b2Vec2 const& point_a, b2Vec2 const& point_b, b2World const& world);
  float32 ReportFixture(b2Fixture* fixture, b2Vec2 const& point, b2Vec2 const& normal, float32 fraction) override;
  typedef BodySet::iterator Iterator;
  Iterator begin();
  Iterator end();
private:
  BodySet impl_;
};
}
#endif