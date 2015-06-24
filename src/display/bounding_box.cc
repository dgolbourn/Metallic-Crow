#include "bounding_box.h"
#include "SDL_sysrender.h"
#include "rect.h"
namespace display
{
class BoundingBox::Impl
{
public:
  Impl() = default;
  Impl(float x, float y, float w, float h);
  Impl(lua::Stack& lua);
  SDL_FRect rect_;
};

auto BoundingBox::operator<(BoundingBox const& other) const -> bool
{
  return impl_.owner_before(other.impl_);
}

BoundingBox::operator bool() const
{
  return bool(impl_);
}

auto BoundingBox::x(float x) -> void
{
  impl_->rect_.x = x;
}

auto BoundingBox::y(float y) -> void
{
  impl_->rect_.y = y;
}

auto BoundingBox::w(float w) -> void
{
  impl_->rect_.w = w;
}

auto BoundingBox::h(float h) -> void
{
  impl_->rect_.h = h;
}

auto BoundingBox::x() const -> float
{
  return impl_->rect_.x;
}

auto BoundingBox::y() const -> float
{
  return impl_->rect_.y;
}

auto BoundingBox::w() const -> float
{
  return impl_->rect_.w;
}

auto BoundingBox::h() const -> float
{
  return impl_->rect_.h;
}

BoundingBox::BoundingBox(float x, float y, float w, float h) : impl_(std::make_shared<Impl>(x, y, w, h))
{
}

BoundingBox::Impl::Impl(float x, float y, float w, float h) : rect_({x, y, w, h})
{
}

BoundingBox::Impl::Impl(lua::Stack& lua)
{
  {
    lua::Guard guard = lua.Field(1);
    lua.Pop(rect_.x);
  }

  {
    lua::Guard guard = lua.Field(2);
    lua.Pop(rect_.y);
  }

  {
    lua::Guard guard = lua.Field(3);
    lua.Pop(rect_.w);
  }

  {
    lua::Guard guard = lua.Field(4);
    lua.Pop(rect_.h);
  }
}

BoundingBox::BoundingBox(lua::Stack& lua)
{
  if(lua.Check())
  {
    impl_ = std::make_shared<Impl>(lua);
  }
}

BoundingBox::BoundingBox(BoundingBox const& first, BoundingBox const& second)
{
  if(bool(first) || bool(second))
  {
    impl_ = std::make_shared<Impl>();

    if(bool(first) && bool(second))
    {
      if(!sdl::Intersection(&first.impl_->rect_, &second.impl_->rect_, &impl_->rect_))
      {
        impl_->rect_.x = .5f * (first.impl_->rect_.x + second.impl_->rect_.x + .5f * (first.impl_->rect_.w + second.impl_->rect_.w));
        impl_->rect_.y = .5f * (first.impl_->rect_.y + second.impl_->rect_.y + .5f * (first.impl_->rect_.h + second.impl_->rect_.h));
        impl_->rect_.w = 0.f;
        impl_->rect_.h = 0.f;
      }
    }
    else if(first)
    {
      impl_->rect_ = first.impl_->rect_;
    }
    else if(second)
    {
      impl_->rect_ = second.impl_->rect_;
    }
  }
}
}