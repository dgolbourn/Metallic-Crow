#ifndef CONTROLLER_H_
#define CONTROLLER_H_
#include <memory>
#include "lua_stack.h"
#include "queue.h"
#include "command.h"
#include "weak_ptr.h"
#include "boost/filesystem.hpp"
#include "timeslice.h"
namespace game
{
class Controller
{
public:
  Controller() = default;
  Controller(lua::Stack& lua, event::Queue& queue, event::Timeslice& loader, boost::filesystem::path const& path);
  auto Control(float x, float y) -> void;
  auto Look(float x, float y) -> void;
  auto ChoiceUp() -> void;
  auto ChoiceDown() -> void;
  auto ChoiceLeft() -> void;
  auto ChoiceRight() -> void;
  auto Select() -> void;
  auto Back() -> void;
  auto Add(event::Command const& command) -> void;
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