#ifndef CONTROLLER_H_
#define CONTROLLER_H_
#include <memory>
#include "json.h"
#include "queue.h"
#include "command.h"
#include "weak_ptr.h"
namespace game
{
class Controller
{
public:
  Controller() = default;
  Controller(json::JSON const& json, event::Queue& queue);
  void Up();
  void Down();
  void Left();
  void Right();
  void ChoiceUp();
  void ChoiceDown();
  void ChoiceLeft();
  void ChoiceRight();
  void Select();
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