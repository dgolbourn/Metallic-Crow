#ifndef MODULATION_H_
#define MODULATION_H_
#include <memory>
namespace display
{
class Modulation
{
public:
  Modulation(void) = default;
  Modulation(float r, float g, float b, float a);
  explicit operator bool(void) const;
  void r(float r);
  void g(float g);
  void b(float b);
  void a(float a);
  float r(void) const;
  float g(void) const;
  float b(void) const;
  float a(void) const;
private:
  std::shared_ptr<class ModulationImpl> impl_;
};
}
#endif