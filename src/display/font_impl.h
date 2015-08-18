#ifndef FONT_IMPL_H_
#define FONT_IMPL_H_
#include "lua_stack.h"
#include "SDL_TTF.h"
#include "ttf_library.h"
#include "boost/filesystem.hpp"
#include <boost/flyweight.hpp>
#include <boost/flyweight/key_value.hpp>
#include "ttf_exception.h"
namespace sdl
{
typedef std::pair<boost::filesystem::path, int> FontKey;

struct FontContainer
{
  TTF_Font* font_;

  FontContainer(FontKey const& key)
  {
    font_ = TTF_OpenFont(key.first.string().c_str(), key.second);
    if(!font_)
    {
      BOOST_THROW_EXCEPTION(ttf::Exception() << ttf::Exception::What(ttf::Error()));
    }
  }

  ~FontContainer()
  {
    if(font_)
    {
      TTF_CloseFont(font_);
    }
  }

  operator TTF_Font*() const
  {
    return font_;
  }
};

typedef boost::flyweight<boost::flyweights::key_value<FontKey, FontContainer>> FlyweightFont;

class Font::Impl
{
public:
  Impl(lua::Stack& lua, boost::filesystem::path const& path);
  auto LineSpacing() const -> float;
  ttf::Library ttf_;
  FlyweightFont font_;
  bool bold_;
  bool italic_;
  bool outline_;
};
}
#endif