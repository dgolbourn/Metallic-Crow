#ifndef FONT_H_
#define FONT_H_
#include <memory>
#include "json.h"
#include "boost/filesystem.hpp"
namespace sdl
{
class Font
{
public:
  Font() = default;
  Font(json::JSON const& json, boost::filesystem::path const& path);
  float LineSpacing() const;
  class Impl;
private:
  std::shared_ptr<Impl> impl_;
  friend class Surface;
};
}
#endif