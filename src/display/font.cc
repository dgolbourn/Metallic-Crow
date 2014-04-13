#include "font.h"
#include "font_impl.h"
#include "ttf_exception.h"
namespace sdl
{
static int Style(bool bold, bool italic)
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

static TTF_Font* Open(std::string const& file, int point)
{
  TTF_Font* font = TTF_OpenFont(file.c_str(), point);
  if(!font)
  {
    BOOST_THROW_EXCEPTION(ttf::Exception() << ttf::Exception::What(ttf::Error()));
  }
  return font;
}

static void Close(TTF_Font* font)
{
  if(font)
  {
    TTF_CloseFont(font);
  }
}

void FontImpl::Init(std::string const& file, int point, int r, int g, int b, int a, bool bold, bool italic, int outline, int or, int og, int ob)
{
  font_ = nullptr;
  outline_ = nullptr;
  try
  {
    colour_ = {r, g, b, a};
    outline_colour_ = {or, og, ob, a};
    font_ = Open(file, point);
    int style = Style(bold, italic);
    TTF_SetFontStyle(font_, style);
    if(outline)
    {
      outline_ = Open(file, point);
      TTF_SetFontStyle(outline_, style);
      TTF_SetFontOutline(outline_, outline);
    }
  }
  catch(...)
  {
    Destroy();
  }
}

void FontImpl::Destroy(void)
{
  Close(font_);
  Close(outline_);
}

FontImpl::FontImpl(std::string const& file, int point, int r, int g, int b, int a, bool bold, bool italic, int outline, int or, int og, int ob)
{
  Init(file, point, r, g, b, a, bold, italic, outline, or, og, ob);
}

FontImpl::FontImpl(json::JSON const& json)
{
  char const* file;
  int point;
  int r;
  int g;
  int b;
  int a;
  int bold;
  int italic;
  int outline;
  int or;
  int og;
  int ob;

  json.Unpack("{sssis[iiii]sisis{sis[iii]}}",
    "file", &file,
    "point", &point,
    "colour", &r, &g, &b, &a,
    "bold", &bold,
    "italic", &italic,
    "outline", 
    "size", &outline,
    "colour", &or, &og, &ob);

  Init(file, point, r, g, b, a, bold != 0, italic != 0, outline, or, og, ob);
}

FontImpl::~FontImpl(void)
{
  Destroy();
}

Font::Font(std::string const& file, int point, int r, int g, int b, int a, bool bold, bool italic, int outline, int or, int og, int ob)
{
  impl_ = std::make_shared<FontImpl>(file, point, r, g, b, a, bold, italic, outline, or, og, ob);
}

Font::Font(json::JSON const& json)
{
  impl_ = std::make_shared<FontImpl>(json);
}
}