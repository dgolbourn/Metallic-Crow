#ifndef FONT_IMPL_H_
#define FONT_IMPL_H_
#include <string>
#include "SDL_TTF.h"
#include "SDL_Pixels.h"
#include "ttf_library.h"
#include "boost/optional.hpp"
namespace sdl
{
typedef boost::optional<SDL_Color> Colour;

class Font::Impl
{
public:
  Impl(json::JSON const& json);
  Impl(std::string const& filename, int point, int r, int g, int b, bool bold, bool italic, bool outline, int or, int ob, int og);
  void Init(std::string const& filename, int point, int r, int g, int b, bool bold, bool italic, Colour const& outline);
  void Destroy();
  float LineSpacing() const;
  ~Impl();
  ttf::Library const ttf_;
  TTF_Font* font_;
  SDL_Color colour_;
  Colour outline_;
};
}
#endif