#ifndef PLAYER_H_
#define PLAYER_H_
#include <memory>
#include <vector>
#include "command.h"
#include "lua_stack.h"
#include "window.h"
#include "boost/filesystem.hpp"
namespace game
{
class Player
{
public:
  Player() = default;
  Player(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path);

  auto Move(int id, float x, float y) -> void;
  auto Look(int id, float x, float y) -> void;
  auto ChoiceUp(int id) -> void;
  auto ChoiceDown(int id) -> void;
  auto ChoiceLeft(int id) -> void;
  auto ChoiceRight(int id) -> void;
  auto ActionLeft(int id, bool state) -> void;
  auto ActionRight(int id, bool state) -> void;
  auto Select(int id) -> void;
  auto Back(int id) -> void;

  typedef std::function<bool(int, float, float)> Control;
  typedef std::function<bool(int)> Button;
  typedef std::function<bool(int, bool)> Switch;

  auto Move(Control const& control) -> void;
  auto Look(Control const& control) -> void;
  auto ChoiceUp(Button const& button) -> void;
  auto ChoiceDown(Button const& button) -> void;
  auto ChoiceLeft(Button const& button) -> void;
  auto ChoiceRight(Button const& button) -> void;
  auto ActionLeft(Switch const& zwitch) -> void;
  auto ActionRight(Switch const& zwitch) -> void;

  auto RawUp(Button const& button) -> void;
  auto RawDown(Button const& button) -> void;

  auto AllUp(event::Command const& command) -> void;
  auto AllDown(event::Command const& command) -> void;
  auto AllSelect(event::Command const& command) -> void;
  auto AllBack(event::Command const& command) -> void;
  auto AllChoiceSelect(event::Command const& command) -> void;
  auto AllChoiceBack(event::Command const& command) -> void;

  auto Add(int id) -> void;
  auto Remove(int id) -> void;

  auto Up(int id) -> void;
  auto Down(int id) -> void;

  auto Render() -> void;

  explicit operator bool();
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif