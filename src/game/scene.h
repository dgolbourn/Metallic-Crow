#ifndef SCENE_H_
#define SCENE_H_
#include <memory>
#include "actor.h"
namespace game
{
class Scene
{
public:
  Scene();
  auto Add(Actor const& actor, float plane) -> void;
  auto Update(Actor const& actor, float plane) -> void;
  auto Remove(Actor const& actor) -> void;
  auto Render() -> void;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif