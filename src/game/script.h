#ifndef SCRIPT_H_
#define SCRIPT_H_
#include <memory>
#include <json.h>
#include "event.h"
#include "window.h"
namespace game
{
class Script
{
public:
  Script(void) = default;
  Script(json::JSON const& json, display::Window& window, event::Queue& queue, event::Event& event);
  void Pause(void);
  void Resume(void);
  void Render(void);
private:
  std::shared_ptr<class ScriptImpl> impl_;
};
}
#endif