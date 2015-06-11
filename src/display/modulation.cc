#include "modulation.h"
namespace display
{
class Modulation::Impl
{
public:
  Impl(float r, float g, float b, float a);
  Impl(lua::Stack& lua);
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

Modulation::Impl::Impl(lua::Stack& lua)
{
  {
    lua::Guard guard = lua.Field(1);
    lua.Pop(r_);
  }

  {
    lua::Guard guard = lua.Field(2);
    lua.Pop(g_);
  }

  {
    lua::Guard guard = lua.Field(3);
    lua.Pop(b_);
  }

  {
    lua::Guard guard = lua.Field(4);
    lua.Pop(a_);
  }
}

Modulation::Modulation(lua::Stack& lua)
{
  if(lua.Check())
  {
    impl_ = std::make_shared<Impl>(lua);
  }
  else
  {
    impl_ = std::make_shared<Impl>(1.f, 1.f, 1.f, 1.f);
  }
}
}