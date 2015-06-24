#ifndef FONT_IMPL_H_
#define FONT_IMPL_H_
#include "lua_stack.h"
#include "SDL_TTF.h"
#include "ttf_library.h"
#include "boost/filesystem.hpp"
namespace sdl
{
class Font::Impl
{
public:
  Impl(lua::Stack& lua, boost::filesystem::path const& path);
  auto LineSpacing() const -> float;
  ttf::Library ttf_;
  typedef std::shared_ptr<TTF_Font> Font;
  Font font_;
  bool bold_;
  bool italic_;
  bool outline_;
};
}
#endif