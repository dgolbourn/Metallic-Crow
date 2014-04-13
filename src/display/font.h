#ifndef FONT_H_
#define FONT_H_
#include <string>
#include <memory>
#include "json.h"
namespace sdl
{
class Font
{
public:
  Font(void) = default;
  Font(std::string const& file, int point, int r, int g, int b, int a = 255, bool bold = false, bool italic = false, int outline = 0, int or = 0, int og = 0, int ob = 0);
  Font(json::JSON const& json);
private:
  std::shared_ptr<class FontImpl> impl_;
  friend class Surface;
};
}
#endif