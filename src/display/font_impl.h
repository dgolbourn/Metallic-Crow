#ifndef FONT_IMPL_H_
#define FONT_IMPL_H_
#include <string>
#include "SDL_TTF.h"
#include "SDL_Pixels.h"
#include "ttf_library.h"
#include "boost/optional.hpp"
#include "boost/filesystem.hpp"
namespace sdl
{
typedef boost::optional<SDL_Colour> Colour;

class Font::Impl
{
public:
  Impl(json::JSON const& json, boost::filesystem::path const& path);
  Impl(boost::filesystem::path const& file, int point, int r, int g, int b, bool bold, bool italic, bool outline, int or, int ob, int og);
  void Init(boost::filesystem::path const& file, int point, int r, int g, int b, bool bold, bool italic, Colour const& outline);
  void Destroy();
  float LineSpacing() const;
  ~Impl();
  ttf::Library ttf_;
  TTF_Font* font_;
  SDL_Colour colour_;
  Colour outline_;
};
}
#endif