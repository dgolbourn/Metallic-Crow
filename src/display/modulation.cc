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

auto Modulation::r(float r) -> void
{
  impl_->r_ = r;
}

auto Modulation::g(float g) -> void
{
  impl_->g_ = g;
}

auto Modulation::b(float b) -> void
{
  impl_->b_ = b;
}

auto Modulation::a(float a) -> void
{
  impl_->a_ = a;
}

auto Modulation::r() const -> float
{
  return impl_->r_;
}

auto Modulation::g() const -> float
{
  return impl_->g_;
}

auto Modulation::b() const -> float
{
  return impl_->b_;
}

auto Modulation::a() const -> float
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