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
  Impl(json::JSON const& json);
  SDL_FRect rect_;
};

bool BoundingBox::operator&(BoundingBox const& other) const
{
  return sdl::Intersection(&impl_->rect_, &other.impl_->rect_);
}

bool BoundingBox::operator<(BoundingBox const& other) const
{
  return impl_.owner_before(other.impl_);
}

BoundingBox::operator bool(void) const
{
  return bool(impl_);
}

void BoundingBox::x(float x)
{
  impl_->rect_.x = x;
}

void BoundingBox::y(float y)
{
  impl_->rect_.y = y;
}

void BoundingBox::w(float w)
{
  impl_->rect_.w = w;
}

void BoundingBox::h(float h)
{
  impl_->rect_.h = h;
}

float BoundingBox::x(void) const
{
  return impl_->rect_.x;
}

float BoundingBox::y(void) const
{
  return impl_->rect_.y;
}

float BoundingBox::w(void) const
{
  return impl_->rect_.w;
}

float BoundingBox::h(void) const
{
  return impl_->rect_.h;
}

BoundingBox::BoundingBox(float x, float y, float w, float h) : impl_(std::make_shared<Impl>(x, y, w, h))
{
}

BoundingBox::Impl::Impl(float x, float y, float w, float h) : rect_({x, y, w, h})
{
}

BoundingBox::Impl::Impl(json::JSON const& json)
{
  double x, y, w, h;
  json.Unpack("[ffff]", &x, &y, &w, &h);
  rect_.x = (float)x;
  rect_.y = (float)y;
  rect_.w = (float)w;
  rect_.h = (float)h;
}

BoundingBox::BoundingBox(json::JSON const& json)
{
  if(json)
  {
    impl_ = std::make_shared<Impl>(json);
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