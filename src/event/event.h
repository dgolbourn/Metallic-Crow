#ifndef EVENT_H_
#define EVENT_H_
#include "json.h"
#include "weak_ptr.h"
#include "command.h"
#include "lua_stack.h"
namespace event
{
class Event
{
public:
  Event() = default;
  Event(lua::Stack& lua);
  Event(json::JSON const& json);
  void operator()();
  typedef std::function<bool(float, float)> Command;
  void Control(Command const& command);
  void ChoiceUp(event::Command const& command);
  void ChoiceDown(event::Command const& command);
  void ChoiceLeft(event::Command const& command);
  void ChoiceRight(event::Command const& command);
  void Select(event::Command const& command);
  void Back(event::Command const& command);
  void Quit(event::Command const& command);
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