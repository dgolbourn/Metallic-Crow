#ifndef BOX_SEARCH_H_
#define BOX_SEARCH_H_
#include "Box2D/Box2D.h"
#include "body_impl.h"
#include <set>
namespace dynamics
{
class BoxSearch final : public b2QueryCallback
{
  typedef std::set<BodyImpl*> BodySet;
public:
  BoxSearch(b2AABB const& box, b2World const& world);
  auto ReportFixture(b2Fixture* fixture) -> bool override;
  typedef BodySet::iterator Iterator;
  auto begin() -> Iterator;
  auto end() -> Iterator;
private:
  BodySet impl_;
};
}
#endif