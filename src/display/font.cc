#include "font.h"
#include "font_impl.h"
#include "ttf_exception.h"
#include <unordered_map>
#include "boost/functional/hash.hpp"
namespace
{ 
typedef std::pair<boost::filesystem::path, int> FontKey;
std::unordered_map<FontKey, std::weak_ptr<TTF_Font>, boost::hash<FontKey>> fonts;

auto Init(boost::filesystem::path const& file, int point) -> TTF_Font*
{
  TTF_Font* font = TTF_OpenFont(file.string().c_str(), point);
  if(!font)
  {
    BOOST_THROW_EXCEPTION(ttf::Exception() << ttf::Exception::What(ttf::Error()));
  }
  return font;
}

auto MakeFont(boost::filesystem::path const& file, int point) -> sdl::Font::Impl::Font
{
  sdl::Font::Impl::Font font;
  auto fileiter = fonts.find(FontKey(file, point));
  if(fileiter != fonts.end())
  {
    font = fileiter->second.lock();
  }
  if(!font)
  {
    font = sdl::Font::Impl::Font(Init(file, point), TTF_CloseFont);
    fonts.emplace(FontKey(file, point), font);
  }
  return font;
}
}

namespace sdl
{
Font::Impl::Impl(lua::Stack& lua, boost::filesystem::path const& path)
{
  std::string file;
  {
    lua::Guard guard = lua.Field("file");
    lua.Pop(file);
  }

  int point;
  {
    lua::Guard guard = lua.Field("point");
    lua.Pop(point);
  }
  font_ = MakeFont(path / file, point);

  {
    lua::Guard guard = lua.Field("bold");
    lua.Pop(bold_);
  }

  {
    lua::Guard guard = lua.Field("italic");
    lua.Pop(italic_);
  }

  {
    lua::Guard guard = lua.Field("outline");
    lua.Pop(outline_);
  }
}

auto Font::Impl::LineSpacing() const -> float
{
  return static_cast<float>(TTF_FontLineSkip(font_.get()));
}

auto Font::LineSpacing() const -> float
{
  return impl_->LineSpacing();
}

Font::Font(lua::Stack& lua, boost::filesystem::path const& path) : impl_(std::make_shared<Impl>(lua, path))
{
}
}