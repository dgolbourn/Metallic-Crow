#ifndef MODULATION_H_
#define MODULATION_H_
#include <memory>
#include "json.h"
namespace display
{
class Modulation
{
public:
  Modulation() = default;
  Modulation(float r, float g, float b, float a);
  Modulation(json::JSON const& json);
  explicit operator bool() const;
  void r(float r);
  void g(float g);
  void b(float b);
  void a(float a);
  float r() const;
  float g() const;
  float b() const;
  float a() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif