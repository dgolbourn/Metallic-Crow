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

SurfacePtr MakeText(TTF_Font *font, char const* text, SDL_Colour const* colour, Uint32 length)
{
  SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(font, text, *colour, length);
  if(!surface)
  {
    BOOST_THROW_EXCEPTION(ttf::Exception() << ttf::Exception::What(ttf::Error()));
  }
  return SurfacePtr(surface, SDL_FreeSurface);
}

SurfacePtr MakeText(TTF_Font *font, char const* text, SDL_Colour const* colour)
{
  SDL_Surface* surface = TTF_RenderText_Blended(font, text, *colour);
  if(!surface)
  {
    BOOST_THROW_EXCEPTION(ttf::Exception() << ttf::Exception::What(ttf::Error()));
  }
  return SurfacePtr(surface, SDL_FreeSurface);
}

class PixelAccess
{
  SDL_Surface* surface_;
  Uint32* pixel_;
public:
  PixelAccess(SDL_Surface* surface) : surface_(surface)
  {
    SDL_LockSurface(surface_);
  }

  ~PixelAccess()
  {
    SDL_UnlockSurface(surface_);
  }

  void Seek(int x, int y)
  {
    pixel_ = (Uint32*)((Uint8*)surface_->pixels + y * surface_->pitch + x * surface_->format->BytesPerPixel);
  }

  void Get(Uint8* r, Uint8* g, Uint8* b, Uint8* a)
  {
    SDL_GetRGBA(*pixel_, surface_->format, r, g, b, a);
  }

  void Set(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
  {
    *pixel_ = SDL_MapRGBA(surface_->format, r, g, b, a);
  }
};

void AddOutline(SDL_Surface* surface, SDL_Colour const* outline)
{
  PixelAccess pixel(surface);

  for(int y = 0; y < surface->h; ++y)
  {
    for(int x = 0; x < surface->w; ++x)
    {
      pixel.Seek(x, y);
      SDL_Colour colour;
      pixel.Get(&colour.r, &colour.g, &colour.b, &colour.a);
      if((colour.a > 0) && (colour.a < 255))
      {
        pixel.Set(outline->r, outline->g, outline->b, colour.a);
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

Surface::Surface(boost::filesystem::path const& file)
{
  SDL_Surface* surface = IMG_Load(file.string().c_str());
  if(!surface)
  {
    BOOST_THROW_EXCEPTION(img::Exception() << img::Exception::What(img::Error()));
  }
  impl_ = SurfacePtr(surface, SDL_FreeSurface);
}

Surface::operator SDL_Surface*() const
{
  return impl_.get();
}
}