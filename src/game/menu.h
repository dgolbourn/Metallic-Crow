#ifndef MENU_H_
#define MENU_H_
#include "json.h"
#include "window.h"
#include <memory>
#include <command.h>
namespace game
{
class Menu
{
public:
  Menu(void) = default;
  Menu(json::JSON const& json, display::Window& window);
  void Add(int selection, event::Command const& command);
  void Previous();
  void Next();
  void Select();
  void Render() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif