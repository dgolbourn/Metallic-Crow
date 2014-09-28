#ifndef STATE_H_
#define STATE_H_
#include <memory>
#include "json.h"
#include "window.h"
#include "command.h"
#include "queue.h"
namespace game
{
class Animation
{
public:
  Animation(json::JSON const& json, display::Window& window, event::Queue& queue);
  Animation(void) = default;
  void Pause(void);
  void Resume(void);
  void Reset(void);
  void Add(event::Command const& command);
  void operator()(display::BoundingBox const& source, display::BoundingBox const& destination, float parallax, bool tile, double angle, display::Modulation const& modulation) const;
  explicit operator bool(void) const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif