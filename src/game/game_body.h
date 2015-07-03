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
  auto Expression(std::string const& expression, bool left_facing) -> double;
  auto Expression(std::string const& expression) -> double;
  auto Expression(bool left_facing) -> double;
  auto Next() -> double;
  auto Period() const -> double;
  auto Position(Position const& position) -> void;
  auto Position() const -> game::Position;
  auto Modulation(float r, float g, float b, float a) -> void;
  auto Modulation() const -> display::Modulation;
  auto Render() const -> void;
  auto Rotation(double angle) -> void;
  auto Rotation() const -> double;
  explicit operator bool() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif