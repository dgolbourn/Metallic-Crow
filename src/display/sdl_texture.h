#ifndef SDL_TEXTURE_H_
#define SDL_TEXTURE_H_
#include "SDL_render.h"
#include <memory>
#include "boost/filesystem.hpp"
namespace sdl
{
class Texture
{
public:
  Texture() = default;
  Texture(SDL_Renderer* renderer, SDL_Surface* surface);
  Texture(SDL_Renderer* renderer, boost::filesystem::path const& file);
  explicit operator SDL_Texture*() const;
  auto operator->() const -> SDL_Texture*;
  explicit operator bool() const;
private:
  std::shared_ptr<SDL_Texture> impl_;
};
}
#endif