#ifndef SCRIPT_H_
#define SCRIPT_H_
#include <memory>
#include <json.h>
#include "event.h"
#include "window.h"
#include "weak_ptr.h"
namespace game
{
class Script
{
public:
  Script(void) = default;
  Script(json::JSON const& json, display::Window& window, event::Queue& queue);
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
  typedef memory::WeakPtr<Script, class ScriptImpl> WeakPtr;
  explicit operator bool(void) const;
private:
  std::shared_ptr<class ScriptImpl> impl_;
  friend WeakPtr;
};
}
#endif