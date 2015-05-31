#ifndef SDL_TEXTURE_H_
#define SDL_TEXTURE_H_
#include "SDL_render.h"
#include <memory>
#include "weak_ptr.h"
#include "boost/filesystem.hpp"
namespace sdl
{
class Texture
{
public:
  Texture() = default;
  Texture(SDL_Renderer* renderer, SDL_Surface* surface);
  Texture::Texture(SDL_Renderer* renderer, boost::filesystem::path const& file);
  explicit operator SDL_Texture*() const;
  SDL_Texture* operator->() const;
  explicit operator bool() const;
  typedef memory::WeakPtr<class Texture, SDL_Texture> WeakPtr;
private:
  std::shared_ptr<SDL_Texture> impl_;
  friend WeakPtr;
};
}
#endif