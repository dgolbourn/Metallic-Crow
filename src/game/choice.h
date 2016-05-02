#ifndef CHOICE_H_
#define CHOICE_H_
#include "lua_stack.h"
#include "window.h"
#include <memory>
#include "queue.h"
#include "weak_ptr.h"
namespace game
{
class Choice
{
public:
  Choice() = default;
  Choice(lua::Stack& lua, display::Window& window, event::Queue& queue, boost::filesystem::path const& path);
  auto operator()(std::string const& up, std::string const& down, std::string const& left, std::string const& right, double timer) -> void;
  auto Up(event::Command const& command) -> void;
  auto Down(event::Command const& command) -> void;
  auto Left(event::Command const& command) -> void;
  auto Right(event::Command const& command) -> void;
  auto Timer(event::Command const& command) -> void;
  auto Up(float r, float g, float b, float a) -> void;
  auto Down(float r, float g, float b, float a) -> void;
  auto Left(float r, float g, float b, float a) -> void;
  auto Right(float r, float g, float b, float a) -> void;
  auto Pause() -> void;
  auto Resume() -> void;
  auto Render() -> void;
  auto Up() -> void;
  auto Down() -> void;
  auto Left() -> void;
  auto Right() -> void;
  explicit operator bool() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
public:
  typedef memory::WeakPtr<Choice> WeakPtr;
  friend WeakPtr;
};
}
#endif