#ifndef CHOICE_H_
#define CHOICE_H_
#include "json.h"
#include "window.h"
#include <memory>
#include "queue.h"
#include "weak_ptr.h"
namespace game
{
class Choice
{
public:
  Choice(void) = default;
  Choice(json::JSON const& json, display::Window& window, event::Queue& queue);
  void operator()(std::string const& up, std::string const& down, std::string const& left, std::string const& right, double timer);
  void Up(event::Command const& command);
  void Down(event::Command const& command);
  void Left(event::Command const& command);
  void Right(event::Command const& command);
  void Timer(event::Command const& command);
  void Pause(void);
  void Resume(void);
  void Render(void) const;
  void Up(void);
  void Down(void);
  void Left(void);
  void Right(void);
  explicit operator bool(void) const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
public:
  typedef memory::WeakPtr<Choice> WeakPtr;
  friend WeakPtr;
};
}
#endif