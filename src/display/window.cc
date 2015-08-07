#include "window.h"
#include "font_impl.h"
#include "surface.h"
#include "render.h"
#include "window_impl.h"
#include "sdl_exception.h"
#include "log.h"
#include "colour.h"
namespace
{
class Modulator
{
  SDL_Colour original_;
  SDL_Renderer* renderer_;
public:
  Modulator(SDL_Renderer* renderer, SDL_Colour const* modulation)
  {
    if(modulation)
    {
      renderer_ = renderer; 
      if(SDL_GetRenderDrawColor(renderer_, &original_.r, &original_.g, &original_.b, &original_.a))
      {
        BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
      }
      if(SDL_SetRenderDrawColor(renderer_, modulation->r, modulation->g, modulation->b, modulation->a))
      {
        BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
      }
    }
  }

  ~Modulator()
  {
    try
    {
      if(SDL_SetRenderDrawColor(renderer_, original_.r, original_.g, original_.b, original_.a))
      {
        BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
      }
    }
    catch(...)
    {
      exception::Log("Swallowed exception");
    }
  }
};

class BlendMode
{
  SDL_BlendMode blend_mode_;
  SDL_Texture* texture_;
public:
  BlendMode(SDL_Texture* texture, SDL_BlendMode blend_mode) : texture_(texture)
  {
    if(SDL_GetTextureBlendMode(texture_, &blend_mode_))
    {
      BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
    }
    if(SDL_SetTextureBlendMode(texture_, blend_mode))
    {
      BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
    }
  }

  ~BlendMode()
  {
    try
    {
      if(SDL_SetTextureBlendMode(texture_, blend_mode_))
      {
        BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
      }
    }
    catch(...)
    {
      exception::Log("Swallowed exception");
    }
  }
};
}

namespace display
{
auto WindowImpl::Destroy() noexcept -> void
{
  if(renderer_)
  {
    SDL_DestroyRenderer(renderer_);
  }
  if(window_)
  {
    SDL_DestroyWindow(window_);
  }
}

WindowImpl::WindowImpl(lua::Stack& lua) : sdl_(SDL_INIT_VIDEO), img_(IMG_INIT_PNG), renderer_(nullptr), window_(nullptr)
{
  try
  {
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    std::string mode;
    {
      lua::Guard guard = lua.Field("mode");
      if(lua.Check())
      {
        lua.Pop(mode);
      }
    }

    Uint32 flags = 0u;
    if(mode == "full screen")
    {
      flags |= SDL_WINDOW_FULLSCREEN;
      SDL_ShowCursor(SDL_DISABLE);
    }
    else if(mode == "borderless")
    {
      flags |= SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED;
    }

    std::string name;
    {
      lua::Guard guard = lua.Field("name");
      lua.Pop(name);
    }

    int width;
    {
      lua::Guard guard = lua.Field("width");
      lua.Pop(width);
    }

    int height;
    {
      lua::Guard guard = lua.Field("height");
      lua.Pop(height);
    }

    window_ = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
    if(!window_)
    {
      BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
    }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer_)
    {
      BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
    }

    if(SDL_SetRenderDrawColor(renderer_, 0, 0, 0, SDL_ALPHA_OPAQUE))
    {
      BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
    }

    if(SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND))
    {
      BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
    }

    view_ = {0.f, 0.f};
    zoom_ = 1.f;
    view_angle_ = {0., 0., 1.0};
    
    int w, h;
    SDL_GetWindowSize(window_, &w, &h);
    scale_ = static_cast<float>(std::min(w, h)) / 1024.f;
  }
  catch(...)
  {
    Destroy();
    throw;
  }
}

WindowImpl::~WindowImpl()
{
  Destroy();
}

auto WindowImpl::Load(boost::filesystem::path const& file) -> sdl::Texture
{
  return sdl::Texture(renderer_, file);
}

auto WindowImpl::Text(std::string const& text, sdl::Font const& font, float length) -> sdl::Texture
{
  return sdl::Texture(renderer_, static_cast<SDL_Surface*>(sdl::Surface(text, font, Uint32(length))));
}

auto WindowImpl::Text(std::string const& text, sdl::Font const& font) -> sdl::Texture
{
  return sdl::Texture(renderer_, static_cast<SDL_Surface*>(sdl::Surface(text, font)));
}

auto WindowImpl::Draw(BoundingBox const& box, Modulation const& modulation) const -> void
{
  SDL_Colour fill = {0, 0, 0, SDL_ALPHA_OPAQUE};
  if(modulation)
  {
    fill.r = sdl::Colour(modulation.r());
    fill.g = sdl::Colour(modulation.g());
    fill.b = sdl::Colour(modulation.b());
    fill.a = sdl::Colour(modulation.a());
  }
  
  SDL_Rect* rect_ptr = nullptr;
  SDL_Rect rect;
  if(box)
  {
    rect.x = static_cast<int>(std::round(box.x()));
    rect.y = static_cast<int>(std::round(box.y()));
    rect.w = static_cast<int>(std::round(box.w()));
    rect.h = static_cast<int>(std::round(box.h()));
    rect_ptr = &rect;
  }

  Modulator modulator(renderer_, &fill);
  if(SDL_RenderFillRect(renderer_, rect_ptr))
  {
    BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
  }
}

auto WindowImpl::Clear() const -> void
{
  if(SDL_RenderClear(renderer_))
  {
    BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
  }
}

auto WindowImpl::Show() const -> void
{
  SDL_RenderPresent(renderer_);
}

auto WindowImpl::View(float x, float y, float zoom) -> void
{
  view_.x = x;
  view_.y = y;
  zoom_ = zoom;
}

auto WindowImpl::Rotation(double angle) -> void
{
  view_angle_.angle_ = angle;
  angle *= M_PI / 180.;
  view_angle_.cos_ = std::cos(angle);
  view_angle_.sin_ = std::sin(angle);
}

auto WindowImpl::Render(sdl::Texture const& texture, BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle, Modulation const& modulation) const -> void
{
  SDL_Rect const* source_ptr = nullptr;
  SDL_Rect source_copy;
  if(source)
  {
    source_copy.x = static_cast<int>(std::round(source.x()));
    source_copy.y = static_cast<int>(std::round(source.y()));
    source_copy.w = static_cast<int>(std::round(source.w()));
    source_copy.h = static_cast<int>(std::round(source.h()));
    source_ptr = &source_copy;
  }

  SDL_FRect const* destination_ptr = nullptr;
  SDL_FRect destination_copy;
  if(destination)
  {
    destination_copy.x = destination.x();
    destination_copy.y = destination.y();
    destination_copy.w = destination.w();
    destination_copy.h = destination.h();
    destination_ptr = &destination_copy;
  }

  if(modulation)
  {
    float r = modulation.r();
    float g = modulation.g();
    float b = modulation.b();
    SDL_Colour modulation_copy;
    modulation_copy.r = sdl::Colour(r--);
    modulation_copy.g = sdl::Colour(g--);
    modulation_copy.b = sdl::Colour(b--);
    modulation_copy.a = sdl::Colour(modulation.a());

    sdl::Render(window_, renderer_, static_cast<SDL_Texture*>(texture), source_ptr, destination_ptr, view_, zoom_, parallax, tile, angle, &modulation_copy, scale_, &view_angle_);

    if((r > 0.f) || (g > 0.f) || (b > 0.f))
    {
      BlendMode blend_mode(static_cast<SDL_Texture*>(texture), SDL_BLENDMODE_ADD);
      do
      {
        modulation_copy.r = sdl::Colour(r--);
        modulation_copy.g = sdl::Colour(g--);
        modulation_copy.b = sdl::Colour(b--);
        sdl::Render(window_, renderer_, static_cast<SDL_Texture*>(texture), source_ptr, destination_ptr, view_, zoom_, parallax, tile, angle, &modulation_copy, scale_, &view_angle_);
      } while((r > 0.f) || (g > 0.f) || (b > 0.f));
    }
  }
  else
  {
    sdl::Render(window_, renderer_, static_cast<SDL_Texture*>(texture), source_ptr, destination_ptr, view_, zoom_, parallax, tile, angle, nullptr, scale_, &view_angle_);
  }
}

Window::Window(lua::Stack& lua) : impl_(std::make_shared<WindowImpl>(lua))
{
}

auto Window::Clear() const -> void
{
  impl_->Clear();
}

auto Window::Show() const -> void
{
  impl_->Show();
}

auto Window::Draw(BoundingBox const& box, Modulation const& modulation) const -> void
{
  impl_->Draw(box, modulation);
}

auto Window::View(float x, float y, float zoom) -> void
{
  impl_->View(x, y, zoom);
}

auto Window::Rotation(double angle) -> void
{
  impl_->Rotation(angle);
}

Window::operator bool() const
{
  return bool(impl_);
}
}