#ifndef FADE_H_
#define FADE_H_
#include <memory>
#include "queue.h"
#include "window.h"
namespace game
{
class Fade
{
public:
  Fade(float period, event::Queue& queue, display::Window const& window);
  void Down();
  void Up();
  void Render() const;
  void Pause();
  void Resume();
  void Add(event::Command const& command);
  void Period(float period);
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif