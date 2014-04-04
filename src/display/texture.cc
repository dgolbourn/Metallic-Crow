#include "texture.h"
#include "sdl_exception.h"

namespace sdl
{
Texture::Texture(SDL_Renderer* renderer, SDL_Surface* surface)
{
  SDL_Texture* impl = SDL_CreateTextureFromSurface(renderer, surface);
  if(!impl)
  {
    BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
  }
  impl_ = std::shared_ptr<SDL_Texture>(impl, SDL_DestroyTexture);
}

Texture::operator SDL_Texture*(void) const
{
  return impl_.get();
}

SDL_Texture* Texture::operator->(void) const
{
  return impl_.get();
}

Texture::operator bool(void) const
{
  return bool(impl_);
}
}