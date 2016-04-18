#include "window.h"
#include "font_impl.h"
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
    std::string vsync = "1";
    {
      lua::Guard guard = lua.Field("vsync");
      if(lua.Check())
      {
        lua.Pop(vsync);
      }
    }
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, vsync.c_str());

    std::string quality = "best";
    {
      lua::Guard guard = lua.Field("quality");
      if(lua.Check())
      {
        lua.Pop(quality);
      }
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, quality.c_str());

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

    std::string name = lua.Field<std::string>("name");
   
    int width = lua.Field<int>("width");

    int height = lua.Field<int>("height");

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
    scale_ = static_cast<float>(std::min(w, h)) / Scale();
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

auto WindowImpl::Draw(BoundingBox const& box, Modulation const& modulation) const -> void
{
  SDL_Colour fill = {0, 0, 0, SDL_ALPHA_OPAQUE};
  if(modulation)
  {
    fill.r = Colour(modulation.r());
    fill.g = Colour(modulation.g());
    fill.b = Colour(modulation.b());
    fill.a = Colour(modulation.a());
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

auto WindowImpl::Scale() const -> float
{
  return 1024.f;
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
  return static_cast<bool>(impl_);
}

auto Window::Scale() const -> float
{
  return impl_->Scale();
}
}