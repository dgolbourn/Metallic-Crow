#ifndef SCREEN_H_
#define SCREEN_H_
#include "scene.h"
#include "json.h"
#include "queue.h"
#include "window.h"
#include "boost/filesystem.hpp"
namespace game
{
class Screen
{
public:
  Screen() = default;
  Screen(json::JSON const& json, display::Window& window, Scene& scene, event::Queue& queue, boost::filesystem::path const& path);
  void Pause();
  void Resume();
  void Modulation(float r, float g, float b, float a);
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif