#include "window.h"
#include "font_impl.h"
#include "surface.h"
#include "render.h"
#include "window_impl.h"
#include "sdl_exception.h"
namespace display
{
namespace
{
Uint8 Colour(float colour)
{
  colour *= 255.f;
  colour = std::round(colour);
  colour = std::min(colour, 255.f);
  colour = std::max(colour, 0.f);
  return Uint8(colour);
}
}

void WindowImpl::Destroy(void)
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

WindowImpl::WindowImpl(json::JSON const& json) : sdl_(SDL_INIT_VIDEO), img_(IMG_INIT_PNG), ttf_()
{
  renderer_ = nullptr;
  window_ = nullptr;
  try
  {
    (void)SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    (void)SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    char const* name;
    int width;
    int height;
    char const* mode;

    json.Unpack("{sssisiss}",
      "name", &name,
      "width", &width,
      "height", &height,
      "mode", &mode);

    Uint32 flags = 0;
    if(std::string(mode) == "full screen")
    {
      flags |= SDL_WINDOW_FULLSCREEN;
    }
    else if(std::string(mode) == "borderless")
    {
      flags |= SDL_WINDOW_BORDERLESS | SDL_WINDOW_MAXIMIZED;
    }

    window_ = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
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

    view_ = {0.f, 0.f};
    zoom_ = 1.f;
  }
  catch(...)
  {
    Destroy();
    throw;
  }
}

WindowImpl::~WindowImpl(void)
{
  Destroy();
}

sdl::Texture WindowImpl::Load(std::string const& file)
{
  sdl::Texture texture;
  auto fileiter = textures_.find(file);
  if(fileiter != textures_.end())
  {
    texture = fileiter->second;
  }
  else
  {
    texture = sdl::Texture(renderer_, (SDL_Surface*)sdl::Surface(file));
    textures_[file] = texture;
  }
  return texture;
}

sdl::Texture WindowImpl::Text(std::string const& text, sdl::Font const& font, float length)
{
  return sdl::Texture(renderer_, (SDL_Surface*)sdl::Surface(text, font, Uint32(length)));
}

sdl::Texture WindowImpl::Text(std::string const& text, sdl::Font const& font)
{
  return sdl::Texture(renderer_, (SDL_Surface*)sdl::Surface(text, font));
}

void WindowImpl::Draw(BoundingBox const& box, Modulation const& modulation) const
{
  SDL_Color colour;
  if(SDL_GetRenderDrawColor(renderer_, &colour.r, &colour.g, &colour.b, &colour.a))
  {
    BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
  }

  SDL_Color fill = {0, 0, 0, 255};
  if(modulation)
  {
    fill.r = Colour(modulation.r());
    fill.g = Colour(modulation.g());
    fill.b = Colour(modulation.b());
    fill.a = Colour(modulation.a());
  }
  if(SDL_SetRenderDrawColor(renderer_, fill.r, fill.g, fill.b, fill.a))
  {
    BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
  }

  SDL_Rect* rect_ptr = nullptr;
  SDL_Rect rect;
  if(box)
  {
    rect.x = int(std::round(box.x()));
    rect.y = int(std::round(box.y()));
    rect.w = int(std::round(box.w()));
    rect.h = int(std::round(box.h()));
    rect_ptr = &rect;
  }

  if(SDL_RenderFillRect(renderer_, rect_ptr))
  {
    BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
  }

  if(SDL_SetRenderDrawColor(renderer_, colour.r, colour.g, colour.b, colour.a))
  {
    BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
  }
}

void WindowImpl::Clear(void) const
{
  if(SDL_RenderClear(renderer_))
  {
    BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
  }
}

void WindowImpl::Show(void) const
{
  SDL_RenderPresent(renderer_);
}

void WindowImpl::Free(void)
{
  textures_.clear();
}

void WindowImpl::View(float x, float y, float zoom)
{
  int w, h;
  SDL_GetWindowSize(window_, &w, &h);
  view_.x = x - .5f * float(w);
  view_.y = y - .5f * float(h);
  zoom_ = zoom;
}

Shape WindowImpl::Shape(void) const
{
  int w, h;
  SDL_GetWindowSize(window_, &w, &h);
  return display::Shape(float(w), float(h));
}

void WindowImpl::Render(sdl::Texture const& texture, BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle, Modulation const& modulation) const
{
  SDL_Rect const* source_ptr = nullptr;
  SDL_Rect source_copy;
  if(source)
  {
    source_copy.x = (int)std::round(source.x());
    source_copy.y = (int)std::round(source.y());
    source_copy.w = (int)std::round(source.w());
    source_copy.h = (int)std::round(source.h());
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

  SDL_Color const* modulation_ptr = nullptr;
  SDL_Color modulation_copy;
  if(modulation)
  {
    modulation_copy.r = Colour(modulation.r());
    modulation_copy.g = Colour(modulation.g());
    modulation_copy.b = Colour(modulation.b());
    modulation_copy.a = Colour(modulation.a());
    modulation_ptr = &modulation_copy;
  }

  sdl::Render(window_, renderer_, (SDL_Texture*)texture, source_ptr, destination_ptr, &view_, zoom_, parallax, tile, angle, modulation_ptr);
}

Shape Window::Shape(void) const
{
  return impl_->Shape();
}

Window::Window(json::JSON const& json)
{
  impl_ = std::make_shared<WindowImpl>(json);
}

void Window::Clear(void) const
{
  return impl_->Clear();
}

void Window::Show(void) const
{
  return impl_->Show();
}

void Window::Draw(BoundingBox const& box, Modulation const& modulation) const
{
  impl_->Draw(box, modulation);
}

void Window::Free(void)
{
  impl_->Free();
}

void Window::View(float x, float y, float zoom)
{
  impl_->View(x, y, zoom);
}

Window::operator bool(void) const
{
  return bool(impl_);
}
}