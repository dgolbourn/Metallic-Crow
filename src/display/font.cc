#include "font.h"
#include "font_impl.h"
#include "ttf_exception.h"
#include <unordered_map>
#include "boost/functional/hash.hpp"
namespace
{ 
typedef std::pair<boost::filesystem::path, int> FontKey;
std::unordered_map<FontKey, std::weak_ptr<TTF_Font>, boost::hash<FontKey>> fonts;

TTF_Font* Init(boost::filesystem::path const& file, int point)
{
  TTF_Font* font = TTF_OpenFont(file.string().c_str(), point);
  if(!font)
  {
    BOOST_THROW_EXCEPTION(ttf::Exception() << ttf::Exception::What(ttf::Error()));
  }
  return font;
}

sdl::Font::Impl::Font MakeFont(boost::filesystem::path const& file, int point)
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
Font::Impl::Impl(json::JSON const& json, boost::filesystem::path const& path)
{
  char const* file;
  int point;
  int bold;
  int italic;
  int outline;
  
  json.Unpack("{sssisbsbsb}",
    "file", &file,
    "point", &point,
    "bold", &bold,
    "italic", &italic,
    "outline", &outline);

  font_ = MakeFont(path / file, point);
  bold_ = (bold != 0);
  italic_ = (italic != 0);
  outline_ = (outline != 0);
}

float Font::Impl::LineSpacing() const
{
  return float(TTF_FontLineSkip(font_.get()));
}

float Font::LineSpacing() const
{
  return impl_->LineSpacing();
}

Font::Font(json::JSON const& json, boost::filesystem::path const& path) : impl_(std::make_shared<Impl>(json, path))
{
}
}