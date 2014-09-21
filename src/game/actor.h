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
namespace game
{
class Actor
{
public:
  Actor(void) = default;
  Actor(json::JSON const& json, display::Window& window, Scene& scene, collision::Group& collision, event::Queue& queue, dynamics::World& world);
  void Position(game::Position const& position);
  game::Position Position(void) const;
  void Velocity(game::Position const& velocity);
  game::Position Velocity(void) const;
  void Force(game::Position const& force);
  void Impulse(game::Position const& impulse);
  void Pause(void);
  void Resume(void);
  void Up(void);
  void Down(void);
  void Left(void);
  void Right(void);
  void Body(std::string const& expression);
  void Eyes(std::string const& expression);
  void Mouth(std::string const& expression);
  void Mouth(int open);
  explicit operator bool(void) const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
public:
  typedef memory::WeakPtr<Actor> WeakPtr;
  friend WeakPtr;
};
}
#endif