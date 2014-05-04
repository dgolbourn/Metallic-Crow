#ifndef SCRIPT_H_
#define SCRIPT_H_
#include <memory>
#include <json.h>
#include "subtitle.h"
#include "scene.h"
#include "dynamics_collision.h"
#include "command_collision.h"
#include "world.h"
#include "event.h"
namespace game
{
class Script
{
public:
  Script(void) = default;
  Script(json::JSON const& json, Subtitle& subtitle, display::Window& window, Scene& scene, event::Queue& queue, DynamicsCollision& dcollision, CommandCollision& ccollision, dynamics::World& world, event::Event& event);
  void Pause(void);
  void Resume(void);
private:
  std::shared_ptr<class ScriptImpl> impl_;
};
}
#endif