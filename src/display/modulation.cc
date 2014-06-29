#include "modulation.h"
namespace display
{
class ModulationImpl
{
public:
  ModulationImpl(float r, float g, float b, float a);
  float r_;
  float g_;
  float b_;
  float a_;
};

ModulationImpl::ModulationImpl(float r, float g, float b, float a) : r_(r), g_(g), b_(b), a_(a)
{
};

void Modulation::r(float r)
{
  impl_->r_ = r;
}

void Modulation::g(float g)
{
  impl_->g_ = g;
}

void Modulation::b(float b)
{
  impl_->b_ = b;
}

void Modulation::a(float a)
{
  impl_->a_ = a;
}

float Modulation::r(void) const
{
  return impl_->r_;
}

float Modulation::g(void) const
{
  return impl_->g_;
}

float Modulation::b(void) const
{
  return impl_->b_;
}

float Modulation::a(void) const
{
  return impl_->a_;
}

Modulation::operator bool(void) const
{
  return bool(impl_);
}

Modulation::Modulation(float r, float g, float b, float a)
{
  impl_ = std::make_shared<ModulationImpl>(r, g, b, a);
}
}