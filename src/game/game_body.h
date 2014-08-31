#ifndef GAME_BODY_H_
#define GAME_BODY_H_
#include <memory>
#include "window.h"
#include "json.h"
#include "position.h"
#include "boost/optional/optional.hpp"
namespace game
{
class Body
{
public:
  Body() = default;
  Body(json::JSON const& json, display::Window& window);
  void Expression(std::string const& expression, bool left_facing);
  void Next();
  typedef boost::optional<Position> OptionalPosition;
  OptionalPosition Eyes() const;
  OptionalPosition Mouth() const;
  void Render(Position const& position, display::Modulation const& modulation, bool front) const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif