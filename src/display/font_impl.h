#ifndef FONT_IMPL_H_
#define FONT_IMPL_H_
#include "SDL_TTF.h"
#include "ttf_library.h"
#include "boost/filesystem.hpp"
namespace sdl
{
class Font::Impl
{
public:
  Impl(json::JSON const& json, boost::filesystem::path const& path);
  float LineSpacing() const;
  ttf::Library ttf_;
  typedef std::shared_ptr<TTF_Font> Font;
  Font font_;
  bool bold_;
  bool italic_;
  bool outline_;
};
}
#endif