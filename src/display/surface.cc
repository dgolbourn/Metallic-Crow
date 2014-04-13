#include "surface.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "ttf_exception.h"
#include "img_exception.h"
#include "sdl_exception.h"
#include "font_impl.h"
namespace sdl
{
typedef std::shared_ptr<SDL_Surface> Text;

Text MakeText(TTF_Font *font, char const* text, SDL_Color colour, Uint32 length)
{
  SDL_Surface* impl = TTF_RenderText_Blended_Wrapped(font, text, colour, length);
  if(!impl)
  {
    BOOST_THROW_EXCEPTION(ttf::Exception() << ttf::Exception::What(ttf::Error()));
  }
  return Text(impl, SDL_FreeSurface);
}

Surface::Surface(std::string const& text, Font const& font, Uint32 length)
{
  Text surface = MakeText(font.impl_->font_, text.c_str(), font.impl_->colour_, length);
  if(font.impl_->outline_)
  {
    Text outline = MakeText(font.impl_->outline_, text.c_str(), font.impl_->outline_colour_, length);
    int offset = TTF_GetFontOutline(font.impl_->outline_);
    SDL_Rect rect = {-offset, -offset};
    if(SDL_BlitSurface(outline.get(), nullptr, surface.get(), &rect))
    {
      BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
    }
  }
  if(SDL_SetSurfaceAlphaMod(surface.get(), font.impl_->colour_.a))
  {
    BOOST_THROW_EXCEPTION(sdl::Exception() << sdl::Exception::What(sdl::Error()));
  }
  impl_ = surface;
}

Surface::Surface(std::string const& file)
{
  SDL_Surface* impl = IMG_Load(file.c_str());
  if(!impl)
  {
    BOOST_THROW_EXCEPTION(img::Exception() << img::Exception::What(img::Error()));
  }
  impl_ = std::shared_ptr<SDL_Surface>(impl, SDL_FreeSurface);
}

Surface::operator SDL_Surface*(void) const
{
  return impl_.get();
}
}