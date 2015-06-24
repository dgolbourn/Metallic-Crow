#ifndef EVENT_H_
#define EVENT_H_
#include "lua_stack.h"
#include "weak_ptr.h"
#include "command.h"
namespace event
{
class Event
{
public:
  Event() = default;
  Event(lua::Stack& lua);
  void operator()();
  typedef std::function<bool(float, float)> Command;
  auto Control(Command const& command) -> void;
  auto ChoiceUp(event::Command const& command) -> void;
  auto ChoiceDown(event::Command const& command) -> void;
  auto ChoiceLeft(event::Command const& command) -> void;
  auto ChoiceRight(event::Command const& command) -> void;
  auto Select(event::Command const& command) -> void;
  auto Back(event::Command const& command) -> void;
  auto Quit(event::Command const& command) -> void;
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