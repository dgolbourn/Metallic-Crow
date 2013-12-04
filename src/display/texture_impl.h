#ifndef TEXTURE_IMPL_H_
#define TEXTURE_IMPL_H_

#include "SDL_rect.h"
#include "SDL_render.h"
#include "bounding_box.h"
#include "window.h"

namespace display
{
class View
{
public:
  float x_;
  float y_;
  float half_width_;
  float half_height_;
  float zoom_;
  int width_;
  int height_;
};

class TextureImpl
{
public:
  TextureImpl(SDL_Texture* texture, SDL_Renderer* renderer, SDL_Window* window, View& view);
  void Render(void) const;
  void Render(SDL_Rect const* source, SDL_Rect const* destination, float parallax, bool tile, double angle) const;
  
  ~TextureImpl(void);

  SDL_Texture* texture_;
  SDL_Renderer* renderer_;
  SDL_Window* window_;
  View& view_;
};
}
#endif