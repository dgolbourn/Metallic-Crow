#ifndef CONTROLLER_H_
#define CONTROLLER_H_
#include <memory>
#include "lua_stack.h"
#include "queue.h"
#include "command.h"
#include "weak_ptr.h"
#include "boost/filesystem.hpp"
namespace game
{
class Controller
{
public:
  Controller() = default;
  Controller(lua::Stack& lua, event::Queue& queue, boost::filesystem::path const& path);
  void Control(float x, float y);
  void ChoiceUp();
  void ChoiceDown();
  void ChoiceLeft();
  void ChoiceRight();
  void Select();
  void Back();
  void Add(event::Command const& command);
  explicit operator bool() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
public:
  typedef memory::WeakPtr<Controller> WeakPtr;
  friend WeakPtr;
};
}
#endif