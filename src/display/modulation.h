#ifndef MODULATION_H_
#define MODULATION_H_
#include <memory>
#include "lua_stack.h"
namespace display
{
class Modulation
{
public:
  Modulation() = default;
  Modulation(float r, float g, float b, float a);
  Modulation(lua::Stack& lua);
  explicit operator bool() const;
  auto r(float r) -> void;
  auto g(float g) -> void;
  auto b(float b) -> void;
  auto a(float a) -> void;
  auto r() const -> float;
  auto g() const -> float;
  auto b() const -> float;
  auto a() const -> float;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif