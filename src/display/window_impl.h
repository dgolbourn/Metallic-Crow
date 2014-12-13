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
#include "boost/filesystem.hpp"
#include "boost/functional/hash.hpp"
namespace display
{
class WindowImpl
{
public:
  WindowImpl(json::JSON const& json);
  sdl::Texture Load(boost::filesystem::path const& file);
  sdl::Texture Text(std::string const& text, sdl::Font const& font, float length);
  sdl::Texture Text(std::string const& text, sdl::Font const& font);
  void Free(boost::filesystem::path const& file);
  void LoadCache(boost::filesystem::path const& file);
  void Clear() const;
  void Show() const;
  void Destroy() noexcept;
  void Draw(BoundingBox const& box, Modulation const& modulation) const;
  void View(float x, float y, float zoom);
  void Render(sdl::Texture const& texture, BoundingBox const& source, BoundingBox const& destination, float parallax, bool tile, double angle, Modulation const& modulation) const;
  ~WindowImpl();
  sdl::Library sdl_;
  img::Library img_;
  ttf::Library ttf_;
  SDL_Window* window_;
  SDL_Renderer* renderer_;
  std::unordered_map<boost::filesystem::path, sdl::Texture::WeakPtr, boost::hash<boost::filesystem::path>> textures_;
  std::unordered_map<boost::filesystem::path, sdl::Texture, boost::hash<boost::filesystem::path>> cache_;
  SDL_FPoint view_;
  float zoom_;
  float scale_;
};
}
#endif