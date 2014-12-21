#ifndef SUBTITLE_H_
#define SUBTITLE_H_
#include "json.h"
#include "window.h"
#include <memory>
#include "boost/filesystem.hpp"
namespace game
{
class Subtitle
{
public:
  Subtitle() = default;
  Subtitle(json::JSON const& json, display::Window& window, boost::filesystem::path const& path);
  void operator()(std::string const& text);
  void Modulation(float r, float g, float b, float a);
  void Render() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif