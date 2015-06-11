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
  bool ReportFixture(b2Fixture* fixture);
  typedef BodySet::iterator Iterator;
  Iterator begin();
  Iterator end();
private:
  BodySet impl_;
};
}
#endif