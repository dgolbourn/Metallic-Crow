#include "surface.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "ttf_exception.h"
#include "img_exception.h"
#include "sdl_exception.h"
#include "font_impl.h"
namespace sdl
{
namespace
{
typedef std::shared_ptr<SDL_Surface> SurfacePtr;

SurfacePtr MakeText(TTF_Font *font, char const* text, SDL_Color const* colour, Uint32 length)
{
  SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, text, *colour, length);
  if(!surface)
  {
    BOOST_THROW_EXCEPTION(ttf::Exception() << ttf::Exception::What(ttf::Error()));
  }
  return SurfacePtr(surface, SDL_FreeSurface);
}

SurfacePtr MakeText(TTF_Font *font, char const* text, SDL_Color const* colour)
{
  SDL_Surface* surface = TTF_RenderText_Blended(font, text, *colour);
  if(!surface)
  {
    BOOST_THROW_EXCEPTION(ttf::Exception() << ttf::Exception::What(ttf::Error()));
  }
  return SurfacePtr(surface, SDL_FreeSurface);
}

Uint32* GetPixel(int x, int y, SDL_Surface const* surface)
{
  return (Uint32*)((Uint8*)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel);
}

class Lock
{
  SDL_Surface* surface_;
public:
  Lock(SDL_Surface* surface) : surface_(surface)
  {
    SDL_LockSurface(surface_);
  }

  ~Lock()
  {
    SDL_UnlockSurface(surface_);
  }
};

void AddOutline(SDL_Surface* surface, SDL_Color const* colour)
{
  Lock lock(surface);

  for(int y = 0; y < surface->h; ++y)
  {
    for(int x = 0; x < surface->w; ++x)
    {
      Uint32* pixel = GetPixel(x, y, surface);

      Uint8 r, g, b, a;
      SDL_GetRGBA(*pixel, surface->format, &r, &g, &b, &a);

      if((a > 0) && (a < 255))
      {
        *pixel = SDL_MapRGBA(surface->format, colour->r, colour->g, colour->b, a);
      }
    }
  }
}
}

Surface::Surface(std::string const& text, Font const& font, Uint32 length)
{
  SurfacePtr surface = MakeText(font.impl_->font_, text.c_str(), &font.impl_->colour_, length);
  if(font.impl_->outline_)
  {
    AddOutline(surface.get(), &(*font.impl_->outline_));
  }
  impl_ = surface;
}

Surface::Surface(std::string const& text, Font const& font)
{
  SurfacePtr surface = MakeText(font.impl_->font_, text.c_str(), &font.impl_->colour_);
  if(font.impl_->outline_)
  {
    AddOutline(surface.get(), &(*font.impl_->outline_));
  }
  impl_ = surface;
}

Surface::Surface(std::string const& file)
{
  SDL_Surface* surface = IMG_Load(file.c_str());
  if(!surface)
  {
    BOOST_THROW_EXCEPTION(img::Exception() << img::Exception::What(img::Error()));
  }
  impl_ = SurfacePtr(surface, SDL_FreeSurface);
}

Surface::operator SDL_Surface*(void) const
{
  return impl_.get();
}
}