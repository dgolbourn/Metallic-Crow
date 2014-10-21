#ifndef EVENT_H_
#define EVENT_H_
#include "switch.h"
#include "json.h"
#include "weak_ptr.h"
namespace event
{
class Event
{
public:
  Event(void) = default;
  Event(json::JSON const& json);
  void operator()(void);
  void Up(Command const& start, Command const& end);
  void Down(Command const& start, Command const& end);
  void Left(Command const& start, Command const& end);
  void Right(Command const& start, Command const& end);
  void ChoiceUp(Command const& start, Command const& end);
  void ChoiceDown(Command const& start, Command const& end);
  void ChoiceLeft(Command const& start, Command const& end);
  void ChoiceRight(Command const& start, Command const& end);
  void Select(Command const& start, Command const& end);
  void Back(Command const& start, Command const& end);
  void Quit(Command const& command);
  explicit operator bool(void) const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
public:
  typedef memory::WeakPtr<Event> WeakPtr;
  friend WeakPtr;
};
}
#endif