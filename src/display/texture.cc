#include "texture.h"
#include "sdl_texture.h"
#include "window.h"
#include "window_impl.h"
#include "boost/filesystem.hpp"
namespace display
{
class Texture::Impl
{
public:
  Impl(boost::filesystem::path const& file, Window& window);
  Impl(std::string const& text, sdl::Font const& font, float width, Window& window);
  Impl(std::string const& text, sdl::Font const& font, Window& window);
  Impl(Impl const& texture, BoundingBox const& clip);
  bool Render(BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle, Modulation const& modulation) const;
  bool Check() const;
  display::Shape Shape() const;
  Window::WeakPtr window_;
  sdl::Texture texture_;
  BoundingBox clip_;
};

Texture::Impl::Impl(boost::filesystem::path const& file, Window& window) : window_(window)
{
  texture_ = window.impl_->Load(file);
  clip_ = BoundingBox(0.f, 0.f, float(texture_->w), float(texture_->h));
}

Texture::Impl::Impl(std::string const& text, sdl::Font const& font, float width, Window& window) : window_(window)
{
  texture_ = window.impl_->Text(text, font, width);
  clip_ = BoundingBox(0.f, 0.f, float(texture_->w), float(texture_->h));
}

Texture::Impl::Impl(std::string const& text, sdl::Font const& font, Window& window) : window_(window)
{
  texture_ = window.impl_->Text(text, font);
  clip_ = BoundingBox(0.f, 0.f, float(texture_->w), float(texture_->h));
}

namespace 
{
BoundingBox Clip(BoundingBox const& current, BoundingBox const& clip)
{
  BoundingBox box;
  if(clip)
  {
    if(current)
    {
      BoundingBox temp(clip, BoundingBox());
      temp.x(temp.x() + current.x());
      temp.y(temp.y() + current.y());
      box = BoundingBox(current, temp);
    }
    else
    {
      box = BoundingBox(clip, BoundingBox());
    }
  }
  else if(current)
  {
    box = BoundingBox(current, BoundingBox());
  }
  return box;
}
}

Texture::Impl::Impl(Impl const& texture, BoundingBox const& clip) : Impl(texture)
{
  clip_ = Clip(clip_, clip);
}

bool Texture::Impl::Render(BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle, Modulation const& modulation) const
{
  bool locked = false;
  if(auto window = window_.Lock())
  {
      window.impl_->Render(texture_, Clip(clip_, source), destination, parallax, tile, angle, modulation);
      locked = true;
  }
  return locked;
}

bool Texture::Impl::Check() const
{
  return bool(window_.Lock());
}

Shape Texture::Impl::Shape() const
{
  return display::Shape(clip_.w(), clip_.h());
}

bool Texture::operator()(BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle, Modulation const& modulation) const
{
  return bool(impl_) && impl_->Render(source, destination, parallax, tile, angle, modulation);
}

Texture::operator bool() const
{
  return bool(impl_) && impl_->Check();
}

Shape Texture::Shape() const
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