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
  void Pause(Command const& command);
  void Resume(Command const& command);
  void Quit(Command const& command);
  void Pause(void);
  void Resume(void);
  explicit operator bool(void) const;
  typedef memory::WeakPtr<Event, class EventImpl> WeakPtr;
private:
  std::shared_ptr<class EventImpl> impl_;
  friend WeakPtr;
};
}
#endif