#include "sdl_texture.h"
#include "sdl_exception.h"
#include "SDL_image.h"
#include "img_exception.h"
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

Texture::Texture(SDL_Renderer* renderer, boost::filesystem::path const& file)
{
  SDL_Texture* impl = IMG_LoadTexture(renderer, file.string().c_str());
  if(!impl)
  {
    BOOST_THROW_EXCEPTION(img::Exception() << img::Exception::What(img::Error()));
  }
  impl_ = std::shared_ptr<SDL_Texture>(impl, SDL_DestroyTexture);
}

Texture::operator SDL_Texture*() const
{
  return impl_.get();
}

SDL_Texture* Texture::operator->() const
{
  return impl_.get();
}

Texture::operator bool() const
{
  return bool(impl_);
}
}