#ifndef ACTOR_H_
#define ACTOR_H_
#include <memory>
#include <string>
#include "window.h"
#include "json.h"
#include "scene.h"
#include "position.h"
#include "weak_ptr.h"
#include "queue.h"
#include "collision_group.h"
#include "world.h"
#include "boost/filesystem.hpp"
namespace game
{
class Actor
{
public:
  Actor() = default;
  Actor(json::JSON const& json, display::Window& window, Scene& scene, collision::Group& collision, event::Queue& queue, dynamics::World& world, boost::filesystem::path const& path);
  void Position(game::Position const& position);
  game::Position Position() const;
  void Velocity(game::Position const& velocity);
  game::Position Velocity() const;
  void Force(game::Position const& force);
  void Impulse(game::Position const& impulse);
  void Pause();
  void Resume();
  void Body(std::string const& expression, bool left_facing);
  void Body(std::string const& expression);
  void Body(bool left_facing);
  void Eyes(std::string const& expression);
  void Mouth(std::string const& expression);
  void Mouth(int open);
  explicit operator bool() const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
public:
  typedef memory::WeakPtr<Actor> WeakPtr;
  friend WeakPtr;
  friend class JointImpl;
};
}
#endif