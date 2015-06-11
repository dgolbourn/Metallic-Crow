#ifndef MENU_H_
#define MENU_H_
#include "lua_stack.h"
#include "window.h"
#include <memory>
#include <command.h>
#include <vector>
#include "boost/filesystem.hpp"
namespace game
{
class Menu
{
public:
  Menu() = default;
  Menu(lua::Stack& lua, display::Window& window, boost::filesystem::path const& path);
  void Add(int index, event::Command const& command);
  void Previous();
  void Next();
  void Select();
  void Render() const;
  typedef std::vector<std::string> Options;
  void operator()(Options const& options);
  void operator[](int index);
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif