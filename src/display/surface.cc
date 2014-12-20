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
class Style
{
  int style_;
  TTF_Font* font_;
public:
  Style(TTF_Font* font, bool bold, bool italic) : font_(font)
  {
    style_ = TTF_GetFontStyle(font);
    int style = 0;
    if(bold)
    {
      style |= TTF_STYLE_BOLD;
    }
    if(italic)
    {
      style |= TTF_STYLE_ITALIC;
    }
    TTF_SetFontStyle(font_, style);
  }
  ~Style()
  {
    TTF_SetFontStyle(font_, style_);
  }
};

typedef std::shared_ptr<SDL_Surface> SurfacePtr;

SurfacePtr MakeText(TTF_Font *font, char const* text, SDL_Colour const* colour, bool bold, bool italic, Uint32 length)
{
  Style style(font, bold, italic);
  SDL_Surface* surface;
  if(length == UINT32_MAX)
  {
    surface = TTF_RenderText_Blended(font, text, *colour);
  }
  else
  {
    surface = TTF_RenderText_Blended_Wrapped(font, text, *colour, length);
  }
  if(!surface)
  {
    BOOST_THROW_EXCEPTION(ttf::Exception() << ttf::Exception::What(ttf::Error()));
  }
  return SurfacePtr(surface, SDL_FreeSurface);
}

SurfacePtr Blank(int w, int h, SDL_PixelFormat const* format)
{
  SDL_Surface* blank = SDL_CreateRGBSurface(0, w, h, format->BitsPerPixel, format->Rmask, format->Gmask, format->Bmask, format->Amask);
  if(!blank)
  {
    BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
  }
  return SurfacePtr(blank, SDL_FreeSurface);
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

SurfacePtr AddOutline(SDL_Surface* surface, SDL_Colour const* outline)
{
  SurfacePtr outlined = Blank(surface->w + 2, surface->h + 2, surface->format);

  {
    PixelAccess pixel_outline(outlined.get());
    PixelAccess pixel_surface(surface);

    for(int y = 0; y < outlined->h; ++y)
    {
      for(int x = 0; x < outlined->w; ++x)
      {
        Uint8 a_max = 0u;
        pixel_outline.Seek(x, y);
        for(int j = -1; j <= 1; ++j)
        {
          for(int i = -1; i <= 1; ++i)
          {
            int p = x + i - 1;
            int q = y + j - 1;
            if((p >= 0) && (q >= 0) && (p < surface->w) && (q < surface->h))
            {
              pixel_surface.Seek(p, q);
              SDL_Colour colour;
              pixel_surface.Get(&colour.r, &colour.g, &colour.b, &colour.a);
              a_max = std::max(a_max, colour.a);
            }
          }
        }
        pixel_outline.Set(outline->r, outline->g, outline->b, a_max);
      }
    }
  }
  
  SDL_Rect dest = {1u, 1u, surface->w, surface->h};
  if(SDL_BlitSurface(surface, nullptr, outlined.get(), &dest))
  {
    BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
  }

  return outlined;
}
}

Surface::Surface(std::string const& text, Font const& font, Uint32 length)
{
  static const SDL_Colour colour = {UINT8_MAX, UINT8_MAX, UINT8_MAX, SDL_ALPHA_OPAQUE};
  SurfacePtr surface = MakeText(font.impl_->font_.get(), text.c_str(), &colour, font.impl_->bold_, font.impl_->italic_, length);
  if(font.impl_->outline_)
  {
    static const SDL_Colour outline = {0, 0, 0, SDL_ALPHA_OPAQUE};
    surface = AddOutline(surface.get(), &outline);
  }
  impl_ = surface;
}

Surface::Surface(std::string const& text, Font const& font) : Surface(text, font, UINT32_MAX)
{
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