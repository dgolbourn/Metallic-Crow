#include "font.h"
#include "font_impl.h"
#include "ttf_exception.h"
namespace sdl
{
namespace
{ 
int Style(bool bold, bool italic)
{
  int style = 0;
  if(bold)
  {
    style |= TTF_STYLE_BOLD;
  }
  if(italic)
  {
    style |= TTF_STYLE_ITALIC;
  }
  return style;
}

TTF_Font* Open(boost::filesystem::path const& file, int point)
{
  TTF_Font* font = TTF_OpenFont(file.string().c_str(), point);
  if(!font)
  {
    BOOST_THROW_EXCEPTION(ttf::Exception() << ttf::Exception::What(ttf::Error()));
  }
  return font;
}

OptionalColour MakeColour(bool valid, int r, int g, int b)
{
  OptionalColour colour;
  if(valid)
  {
    colour = {Uint8(r), Uint8(g), Uint8(b), 255u};
  }
  return colour;
}

OptionalColour MakeColour(json::JSON const& json)
{
  OptionalColour colour;
  if(json)
  {
    int r, g, b;
    json.Unpack("[iii]", &r, &g, &b);
    colour = MakeColour(true, r, g, b);
  }
  return colour;
}
}

void Font::Impl::Init(boost::filesystem::path const& file, int point, int r, int g, int b, bool bold, bool italic, OptionalColour const& outline)
{
  font_ = nullptr;
  try
  {
    colour_ = {Uint8(r), Uint8(g), Uint8(b), 255u};
    outline_ = outline;
    font_ = Open(file, point);
    TTF_SetFontStyle(font_, Style(bold, italic));
  }
  catch(...)
  {
    Destroy();
  }
}

void Font::Impl::Destroy(void)
{
  if(font_)
  {
    TTF_CloseFont(font_);
  }
}

Font::Impl::Impl(boost::filesystem::path const& file, int point, int r, int g, int b, bool bold, bool italic, bool outline, int or, int og, int ob)
{
  Init(file, point, r, g, b, bold, italic, MakeColour(outline, or, og, ob));
}

Font::Impl::Impl(json::JSON const& json, boost::filesystem::path const& path)
{
  char const* file;
  int point;
  int r;
  int g;
  int b;
  int bold;
  int italic;
  json_t* outline;

  json.Unpack("{sssis[iii]sbsbso}",
    "file", &file,
    "point", &point,
    "colour", &r, &g, &b,
    "bold", &bold,
    "italic", &italic,
    "outline", &outline);

  Init(path / file, point, r, g, b, bold != 0, italic != 0, MakeColour(json::JSON(outline)));
}

Font::Impl::~Impl(void)
{
  Destroy();
}

float Font::Impl::LineSpacing() const
{
  return float(TTF_FontLineSkip(font_));
}

float Font::LineSpacing() const
{
  return impl_->LineSpacing();
}

Font::Font(boost::filesystem::path const& file, int point, int r, int g, int b, bool bold, bool italic, bool outline, int or, int og, int ob) : impl_(std::make_shared<Impl>(file, point, r, g, b, bold, italic, outline, or, og, ob))
{
}

Font::Font(json::JSON const& json, boost::filesystem::path const& path) : impl_(std::make_shared<Impl>(json, path))
{
}
}