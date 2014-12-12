#ifndef GAME_BODY_H_
#define GAME_BODY_H_
#include <memory>
#include "window.h"
#include "json.h"
#include "position.h"
#include "boost/filesystem.hpp"
#include "modulation.h"
#include "bounding_box.h"
#include "feature.h"
namespace game
{
class Body
{
public:
  Body() = default;
  Body(json::JSON const& json, display::Window& window, boost::filesystem::path const& path, Feature const& eyes, Feature const& mouth);
  void Expression(std::string const& expression, bool left_facing);
  void Expression(std::string const& expression);
  void Expression(bool left_facing);
  void Next();
  void Position(Position const& position);
  game::Position Position() const;
  void Modulation(display::Modulation const& modulation);
  void Render();
  explicit operator bool() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif