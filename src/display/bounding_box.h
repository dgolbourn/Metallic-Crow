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
  auto operator<(BoundingBox const& other) const -> bool;
  explicit operator bool() const;
  auto x(float x) -> void;
  auto y(float y) -> void;
  auto w(float w) -> void;
  auto h(float h) -> void;
  auto x() const -> float;
  auto y() const -> float;
  auto w() const -> float;
  auto h() const -> float;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
public:
  typedef memory::WeakPtr<BoundingBox> WeakPtr;
  friend WeakPtr;
};
}
#endif