#ifndef WINDOW_IMPL_H_
#define WINDOW_IMPl_H_
#include "json.h"
#include "sdl_texture.h"
#include <string>
#include "font.h"
#include "bounding_box.h"
#include <unordered_map>
#include "SDL_image.h"
#include "sdl_library.h"
#include "img_library.h"
#include "ttf_library.h"
#include "SDL_sysrender.h"
#include "shape.h"
namespace display
{
class WindowImpl
{
public:
  WindowImpl(json::JSON const& json);
  sdl::Texture Load(std::string const& file);
  sdl::Texture Text(std::string const& text, sdl::Font const& font, float length);
  sdl::Texture Text(std::string const& text, sdl::Font const& font);
  void Free(void);
  void Clear(void) const;
  void Show(void) const;
  void Destroy(void);
  void Draw(BoundingBox const& box, Modulation const& modulation) const;
  void View(float x, float y, float zoom);
  void Render(sdl::Texture const& texture, BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle, Modulation const& modulation) const;
  Shape Shape(void) const;
  ~WindowImpl(void);
  sdl::Library const sdl_;
  img::Library const img_;
  ttf::Library const ttf_;
  SDL_Window* window_;
  SDL_Renderer* renderer_;
  std::unordered_map<std::string, sdl::Texture> textures_;
  SDL_FPoint view_;
  float zoom_;
};
}
#endif