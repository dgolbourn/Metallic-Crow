#include "surface.h"
#include "SDL_image.h"
#include "ttf_exception.h"
#include "img_exception.h"
#include "sdl_exception.h"

namespace sdl
{
Surface::Surface(TTF_Font* font, char const* text, SDL_Color colour)
{
  SDL_Surface* impl = TTF_RenderText_Blended(font, text, colour);
  if(!impl)
  {
    BOOST_THROW_EXCEPTION(ttf::Exception() << ttf::Exception::What(ttf::Error()));
  }
  impl_ = std::shared_ptr<SDL_Surface>(impl, SDL_FreeSurface);
}

Surface::Surface(char const* file)
{
  SDL_Surface* impl = IMG_Load(file);
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

SDL_Surface* Surface::operator->(void) const
{
  return impl_.get();
}
}