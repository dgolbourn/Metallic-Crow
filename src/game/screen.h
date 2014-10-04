#ifndef SCREEN_H_
#define SCREEN_H_
#include "scene.h"
#include "json.h"
#include "queue.h"
#include "window.h"
namespace game
{
class Screen
{
public:
  Screen() = default;
  Screen(json::JSON const& json, display::Window& window, Scene& scene, event::Queue& queue);
  void Pause();
  void Resume();
  void Modulation(float r, float g, float b);
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif