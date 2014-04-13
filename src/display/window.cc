#include "window.h"
#include <unordered_map>
#include "SDL_image.h"
#include "sdl_library.h"
#include "img_library.h"
#include "ttf_library.h"
#include "sdl_exception.h"
#include "texture.h"
#include "font_impl.h"
#include "surface.h"
#include "render.h"
#include "SDL_sysrender.h"

namespace display
{
class WindowImpl
{
public:
  WindowImpl(json::JSON const& json);
  sdl::Texture::WeakPtr Load(std::string const& filename);
  sdl::Texture Text(std::string const& text, sdl::Font const& font);
  void Free(void);
  void Clear(void) const;
  void Show(void) const;
  void Destroy(void);
  void View(float x, float y, float zoom);

  ~WindowImpl(void);

  sdl::Library const sdl_;
  img::Library const img_;
  ttf::Library const ttf_;
  SDL_Window* window_;
  SDL_Renderer* renderer_;
  std::unordered_map<std::string, sdl::Texture> textures_;
  SDL_FPoint view_;
  float zoom_;
};

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
    if(!strcmp(mode, "fullscreen"))
    {
      flags |= SDL_WINDOW_FULLSCREEN;
    }
    else if(!strcmp(mode, "borderless"))
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

sdl::Texture::WeakPtr WindowImpl::Load(std::string const& filename)
{
  sdl::Texture::WeakPtr texture_ptr;
  auto fileiter = textures_.find(filename);
  if(fileiter != textures_.end())
  {
    texture_ptr = fileiter->second;
  }
  else
  {
    sdl::Texture texture(renderer_, sdl::Surface(filename.c_str()));
    textures_[filename] = texture;
    texture_ptr = texture;
  }
  return texture_ptr;
}

sdl::Texture WindowImpl::Text(std::string const& text, sdl::Font const& font)
{
  return sdl::Texture(renderer_, sdl::Surface(text, font));
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
  int w;
  int h;
  SDL_GetWindowSize(window_, &w, &h);
  view_.x = x - .5f * (float)w;
  view_.y = y - .5f * (float)h;
  zoom_ = zoom;
}

Window::Window(json::JSON const& json)
{
  impl_ = std::make_shared<WindowImpl>(json);
}

static void Render(std::shared_ptr<WindowImpl> const& window, sdl::Texture const& texture, BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle)
{
  SDL_Rect* source_ptr = nullptr;
  SDL_Rect source_copy;
  if(source)
  {
    source_copy.x = (int)std::round(source.x());
    source_copy.y = (int)std::round(source.y());
    source_copy.w = (int)std::round(source.w());
    source_copy.h = (int)std::round(source.h());
    source_ptr = &source_copy;
  }

  SDL_FRect* destination_ptr = nullptr;
  SDL_FRect destination_copy;
  if(destination)
  {
    destination_copy.x = destination.x();
    destination_copy.y = destination.y();
    destination_copy.w = destination.w();
    destination_copy.h = destination.h();
    destination_ptr = &destination_copy;
  }

  sdl::Render(window->window_, window->renderer_, texture, source_ptr, destination_ptr, &window->view_, window->zoom_, parallax, tile, angle);
}

static Texture Bind(std::weak_ptr<WindowImpl> window_ptr, sdl::Texture::WeakPtr texture_ptr)
{
  return [window_ptr, texture_ptr](BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle)
  {
    bool locked = false;
    if(auto window = window_ptr.lock())
    {
      if(auto texture = texture_ptr.Lock())
      {
        Render(window, texture, source, destination, parallax, tile, angle);
        locked = true;
      }
    }
    return locked;
  };
}

static Texture Bind(std::weak_ptr<WindowImpl> window_ptr, sdl::Texture texture)
{
  return [window_ptr, texture](BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle)
  {
    bool locked = false;
    if(auto window = window_ptr.lock())
    {
      Render(window, texture, source, destination, parallax, tile, angle);
      locked = true;
    }
    return locked;
  };
}

Texture Window::Load(std::string const& filename)
{
  return Bind(impl_, impl_->Load(filename));
}

Texture Window::Text(std::string const& text, sdl::Font const& font)
{
  return Bind(impl_, impl_->Text(text, font));
}

void Window::Clear(void) const
{
  return impl_->Clear();
}

void Window::Show(void) const
{
  return impl_->Show();
}

void Window::Free(void)
{
  impl_->Free();
}

void Window::View(float x, float y, float zoom)
{
  impl_->View(x, y, zoom);
}
}