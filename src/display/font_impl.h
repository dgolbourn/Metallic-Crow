#ifndef FONT_IMPL_H_
#define FONT_IMPL_H_
#include <string>
#include "SDL_TTF.h"
#include "SDL_Pixels.h"
#include "ttf_library.h"
namespace sdl
{
class FontImpl
{
public:
  FontImpl(json::JSON const& json);
  FontImpl(std::string const& filename, int point, int r, int g, int b, int a, bool bold, bool italic, int outline, int or, int ob, int og);
  void Init(std::string const& filename, int point, int r, int g, int b, int a, bool bold, bool italic, int outline, int or, int ob, int og);
  void Destroy(void);
  ~FontImpl(void);
  ttf::Library const ttf_;
  TTF_Font* font_;
  SDL_Color colour_;
  TTF_Font* outline_;
  SDL_Color outline_colour_;
};
}
#endif