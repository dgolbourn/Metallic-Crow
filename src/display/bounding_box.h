#ifndef BOUNDING_BOX_H_
#define BOUNDING_BOX_H_
#include <memory>
#include "lua_stack.h"
#include "weak_ptr.h"
namespace display
{
class BoundingBox
{
public:
  BoundingBox() = default;
  BoundingBox(float x, float y, float w, float h);
  BoundingBox(lua::Stack& lua);
  BoundingBox(BoundingBox const& first, BoundingBox const& second);
  bool operator<(BoundingBox const& other) const;
  explicit operator bool() const;
  void x(float x);
  void y(float y);
  void w(float w);
  void h(float h);
  float x() const;
  float y() const;
  float w() const;
  float h() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
public:
  typedef memory::WeakPtr<BoundingBox> WeakPtr;
  friend WeakPtr;
};
}
#endif