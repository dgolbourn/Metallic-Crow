#ifndef GAME_BODY_H_
#define GAME_BODY_H_
#include <memory>
#include "window.h"
#include "lua_stack.h"
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
  Body(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path, Feature const& eyes, Feature const& mouth);
  double Expression(std::string const& expression, bool left_facing);
  double Expression(std::string const& expression);
  double Expression(bool left_facing);
  double Next();
  double Period() const;
  void Position(Position const& position);
  game::Position Position() const;
  void Modulation(float r, float g, float b, float a);
  display::Modulation Modulation() const;
  void Render();
  explicit operator bool() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif