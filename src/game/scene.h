#ifndef SCENE_H_
#define SCENE_H_
#include <memory>
#include "command.h"
namespace game
{
class Scene
{
public:
  Scene();
  void Add(event::Command const& layer, int plane);
  void Render();
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif