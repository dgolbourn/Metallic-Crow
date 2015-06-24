#include "texture.h"
#include "sdl_texture.h"
#include "window.h"
#include "window_impl.h"
#include "boost/filesystem.hpp"
namespace 
{
auto Clip(display::BoundingBox const& current, display::BoundingBox const& clip, sdl::Texture const& texture) -> display::BoundingBox
{
  display::BoundingBox box;
  if(clip)
  {
    if(current)
    {
      display::BoundingBox temp(clip, display::BoundingBox());
      temp.x(temp.x() + current.x());
      temp.y(temp.y() + current.y());
      box = display::BoundingBox(current, temp);
    }
    else
    {
      box = display::BoundingBox(display::BoundingBox(0.f, 0.f, static_cast<float>(texture->w), static_cast<float>(texture->h)), clip);
    }
  }
  else if(current)
  {
    box = display::BoundingBox(current, display::BoundingBox());
  }
  return box;
}
}

namespace display
{
class Texture::Impl
{
public:
  Impl(boost::filesystem::path const& file, Window& window);
  Impl(std::string const& text, sdl::Font const& font, float width, Window& window);
  Impl(std::string const& text, sdl::Font const& font, Window& window);
  Impl(Impl const& texture, BoundingBox const& clip);
  auto Render(BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle, Modulation const& modulation) const -> bool;
  auto Check() const -> bool;
  display::Shape Shape() const;
  Window::WeakPtr window_;
  sdl::Texture texture_;
  BoundingBox clip_;
};

Texture::Impl::Impl(boost::filesystem::path const& file, Window& window) : window_(window)
{
  texture_ = window.impl_->Load(file);
}

Texture::Impl::Impl(std::string const& text, sdl::Font const& font, float width, Window& window) : window_(window)
{
  texture_ = window.impl_->Text(text, font, width);
}

Texture::Impl::Impl(std::string const& text, sdl::Font const& font, Window& window) : window_(window)
{
  texture_ = window.impl_->Text(text, font);
}

Texture::Impl::Impl(Impl const& texture, BoundingBox const& clip) : Impl(texture)
{
  clip_ = Clip(clip_, clip, texture_);
}

auto Texture::Impl::Render(BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle, Modulation const& modulation) const -> bool
{
  bool locked = false;
  if(auto window = window_.Lock())
  {
    if(texture_)
    {
      window.impl_->Render(texture_, Clip(clip_, source, texture_), destination, parallax, tile, angle, modulation);
    }
    locked = true;
  }
  return locked;
}

auto Texture::Impl::Check() const -> bool
{
  return bool(window_.Lock());
}

auto Texture::Impl::Shape() const -> display::Shape
{
  display::Shape shape;
  if(clip_)
  {
    shape = display::Shape(clip_.w(), clip_.h());
  }
  else if(texture_)
  {
    shape = display::Shape(static_cast<float>(texture_->w), static_cast<float>(texture_->h));
  }
  else
  {
    shape = display::Shape(0.f, 0.f);
  }
  return shape;
}

auto Texture::operator()(BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle, Modulation const& modulation) const -> bool
{
  return bool(impl_) && impl_->Render(source, destination, parallax, tile, angle, modulation);
}

Texture::operator bool() const
{
  return bool(impl_) && impl_->Check();
}

auto Texture::Shape() const -> display::Shape
{
  return impl_->Shape();
}

Texture::Texture(boost::filesystem::path const& file, Window& window) : impl_(std::make_shared<Impl>(file, window))
{
}

Texture::Texture(std::string const& text, sdl::Font const& font, float width, Window& window) : impl_(std::make_shared<Impl>(text, font, width, window))
{
}

Texture::Texture(std::string const& text, sdl::Font const& font, Window& window) : impl_(std::make_shared<Impl>(text, font, window))
{
}

Texture::Texture(Texture const& texture, BoundingBox const& clip) : impl_(std::make_shared<Impl>(*texture.impl_, clip))
{
}
}