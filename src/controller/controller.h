#ifndef CONTROLLER_H_
#define CONTROLLER_H_
#include <memory>
#include <vector>
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
  auto Quit(event::Command const& command) -> void;
  auto Add(int id) -> void;
  auto Remove(int id) -> void;
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