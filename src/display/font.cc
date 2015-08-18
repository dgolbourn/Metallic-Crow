#include "font.h"
#include "font_impl.h"
namespace sdl
{
Font::Impl::Impl(lua::Stack& lua, boost::filesystem::path const& path) : font_(std::make_pair(path / lua.Field<std::string>("file"), lua.Field<int>("point")))
{
  bold_ = lua.Field<bool>("bold");
  italic_ = lua.Field<bool>("italic");
  outline_ = lua.Field<bool>("outline");
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