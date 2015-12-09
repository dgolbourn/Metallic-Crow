#ifndef WINDOW_IMPL_H_
#define WINDOW_IMPl_H_
#include "lua_stack.h"
#include "sdl_texture.h"
#include <string>
#include "font.h"
#include "bounding_box.h"
#include "SDL_image.h"
#include "sdl_library.h"
#include "img_library.h"
#include "ttf_library.h"
#include "SDL_sysrender.h"
#include "boost/filesystem.hpp"
#include "boost/functional/hash.hpp"
#include "render.h"
namespace display
{
class WindowImpl
{
public:
  WindowImpl(lua::Stack& lua);
  auto Clear() const -> void;
  auto Show() const -> void;
  auto Destroy() noexcept -> void;
  auto Draw(BoundingBox const& box, Modulation const& modulation) const -> void;
  auto View(float x, float y, float zoom) -> void;
  auto Rotation(double angle) -> void;
  auto Scale() const -> float;
  ~WindowImpl();
  sdl::Library sdl_;
  img::Library img_;
  ttf::Library ttf_;
  SDL_Window* window_;
  SDL_Renderer* renderer_;
  SDL_FPoint view_;
  float zoom_;
  float scale_;
  Angle view_angle_;
};
}
#endif