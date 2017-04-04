#ifndef EVENT_H_
#define EVENT_H_
#include "lua_stack.h"
#include "weak_ptr.h"
#include "command.h"
#include <vector>
namespace event
{
class Event
{
public:
  Event() = default;
  Event(lua::Stack& lua);
  void operator()();
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
  auto Select(Button const& button) -> void;
  auto Back(Button const& button) -> void;
  auto Quit(Command const& command) -> void;
  auto Controllers() -> std::vector<int>;
  typedef std::function<bool(int)> Index;
  auto Add(Index const& id) -> void;
  auto Remove(Index const& id) -> void;
  explicit operator bool() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
public:
  typedef memory::WeakPtr<Event> WeakPtr;
  friend WeakPtr;
};
}
#endif