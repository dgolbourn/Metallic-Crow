#ifndef MENU_H_
#define MENU_H_
#include "json.h"
#include "window.h"
#include <memory>
#include <command.h>
#include <vector>
namespace game
{
class Menu
{
public:
  Menu() = default;
  Menu(json::JSON const& json, display::Window& window);
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