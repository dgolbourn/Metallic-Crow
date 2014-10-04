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
  Fade(event::Queue& queue, display::Window const& window);
  void Down(float period);
  void Up(float period);
  void Render() const;
  void Pause();
  void Resume();
  void Add(event::Command const& command);
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif