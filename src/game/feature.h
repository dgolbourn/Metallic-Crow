#ifndef FEATURE_H_
#define FEATURE_H_
#include <memory>
#include "window.h"
#include "json.h"
#include "position.h"
#include "boost/filesystem.hpp"
namespace game
{
class Feature
{
public:
  Feature() = default;
  Feature(json::JSON const& json, display::Window& window, boost::filesystem::path const& path);
  void Expression(std::string const& expression, int index, bool left_facing);
  void Render(Position const& position, display::Modulation const& modulation) const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif