#include "texture.h"
#include "sdl_texture.h"
#include "window.h"
#include "window_impl.h"
namespace display
{
class TextureImpl
{
public:
  TextureImpl(std::string const& file, Window& window);
  TextureImpl(std::string const& text, sdl::Font const& font, int length, Window& window);
  TextureImpl(std::string const& text, sdl::Font const& font, Window& window);
  bool Render(display::BoundingBox const& source, display::BoundingBox const& destination, float parallax, bool tile, double angle);
  bool Check(void);
  Window::WeakPtr window_;
  sdl::Texture::WeakPtr texture_ptr_;
  sdl::Texture texture_;
};

TextureImpl::TextureImpl(std::string const& file, Window& window) : window_(window)
{
  texture_ptr_ = window.impl_->Load(file);
}

TextureImpl::TextureImpl(std::string const& text, sdl::Font const& font, int length, Window& window) : window_(window)
{
  texture_ = window.impl_->Text(text, font, length);
  texture_ptr_ = texture_;
}

TextureImpl::TextureImpl(std::string const& text, sdl::Font const& font, Window& window) : window_(window)
{
  texture_ = window.impl_->Text(text, font);
  texture_ptr_ = texture_;
}

bool TextureImpl::Render(display::BoundingBox const& source, display::BoundingBox const& destination, float parallax, bool tile, double angle)
{
  bool locked = false;
  if(auto window = window_.Lock())
  {
    if(auto texture = texture_ptr_.Lock())
    {
      window.impl_->Render(texture, source, destination, parallax, tile, angle);
      locked = true;
    }
  }
  return locked;
}

bool TextureImpl::Check(void)
{
  return bool(window_.Lock()) && bool(texture_ptr_.Lock());
}

bool Texture::operator()(display::BoundingBox const& source, display::BoundingBox const& destination, float parallax, bool tile, double angle)
{
  return impl_->Render(source, destination, parallax, tile, angle);
}

Texture::operator bool(void) const
{
  return bool(impl_) && impl_->Check();
}

Texture::Texture(std::string const& file, Window& window)
{
  impl_ = std::make_shared<TextureImpl>(file, window);
}

Texture::Texture(std::string const& text, sdl::Font const& font, int length, Window& window)
{
  impl_ = std::make_shared<TextureImpl>(text, font, length, window);
}

Texture::Texture(std::string const& text, sdl::Font const& font, Window& window)
{
  impl_ = std::make_shared<TextureImpl>(text, font, window);
}
}