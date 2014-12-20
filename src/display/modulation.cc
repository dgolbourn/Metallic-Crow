#include "modulation.h"
namespace display
{
class Modulation::Impl
{
public:
  Impl(float r, float g, float b, float a);
  Impl(json::JSON const& json);
  float r_;
  float g_;
  float b_;
  float a_;
};

Modulation::Impl::Impl(float r, float g, float b, float a) : r_(r), g_(g), b_(b), a_(a)
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

float Modulation::r() const
{
  return impl_->r_;
}

float Modulation::g() const
{
  return impl_->g_;
}

float Modulation::b() const
{
  return impl_->b_;
}

float Modulation::a() const
{
  return impl_->a_;
}

Modulation::operator bool() const
{
  return bool(impl_);
}

Modulation::Modulation(float r, float g, float b, float a) : impl_(std::make_shared<Impl>(r, g, b, a))
{
}

Modulation::Impl::Impl(json::JSON const& json)
{
  double r, g, b, a;
  json.Unpack("[ffff]", &r, &g, &b, &a);
  r_ = (float)r;
  g_ = (float)g;
  b_ = (float)b;
  a_ = (float)a;
}

Modulation::Modulation(json::JSON const& json)
{
  if(json)
  {
    impl_ = std::make_shared<Impl>(json);
  }
}
}