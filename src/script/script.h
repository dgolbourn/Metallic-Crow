#ifndef SCRIPT_H_
#define SCRIPT_H_
#include <memory>
#include <string>
#include "window.h"
#include "weak_ptr.h"
#include "queue.h"
namespace game
{
class Script
{
public:
  Script(void) = default;
  Script(std::string const& file, display::Window& window, event::Queue& queue);
  void Pause(void);
  void Resume(void);
  void Render(void);
  void ChoiceUp(void);
  void ChoiceDown(void);
  void ChoiceLeft(void);
  void ChoiceRight(void);
  void Up(void);
  void Down(void);
  void Left(void);
  void Right(void);
  explicit operator bool(void) const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
public:
  typedef memory::WeakPtr<Script> WeakPtr;
  friend WeakPtr;
};
}
#endif