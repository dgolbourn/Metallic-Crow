#ifndef MENU_H_
#define MENU_H_
#include "lua_stack.h"
#include "window.h"
#include <memory>
#include <command.h>
#include <vector>
#include "boost/filesystem.hpp"
#include "timeslice.h"
namespace game
{
class Menu
{
public:
  Menu() = default;
  Menu(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path, event::Timeslice& loader);
  auto Add(int index, event::Command const& command) -> void;
  auto Previous() -> void;
  auto Next() -> void;
  auto Select() -> void;
  auto Render() -> void;
  typedef std::vector<std::string> Options;
  auto operator()(Options const& options) -> void;
  auto operator[](int index) -> void;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif