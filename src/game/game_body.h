#ifndef GAME_BODY_H_
#define GAME_BODY_H_
#include <memory>
#include "window.h"
#include "json.h"
#include "position.h"
#include "boost/optional/optional.hpp"
#include "command.h"
#include "boost/filesystem.hpp"
namespace game
{
class Body
{
public:
  Body() = default;
  Body(json::JSON const& json, display::Window& window, boost::filesystem::path const& path);
  void Expression(std::string const& expression, bool left_facing);
  void Next();
  typedef boost::optional<Position> OptionalPosition;
  OptionalPosition Eyes() const;
  OptionalPosition Mouth() const;
  void Facing(event::Command const& command);
  void Render(Position const& position, display::Modulation const& modulation, bool front) const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif