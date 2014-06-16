#ifndef HERO_H_
#define HERO_H_
#include <memory>
#include <string>
#include "window.h"
#include "json.h"
#include "scene.h"
#include "position.h"
#include "weak_ptr.h"
#include "queue.h"
#include "dynamics_collision.h"
#include "world.h"
#include "command_collision.h"
namespace game
{
class Hero
{
public:
  Hero(void) = default;
  Hero(json::JSON const& json, display::Window& window, Scene& scene, DynamicsCollision& dcollision, CommandCollision& ccollision, event::Queue& queue, dynamics::World& world);
  void Position(game::Position const& position);
  game::Position Position(void);
  void Pause(void);
  void Resume(void);
  void Up(void);
  void Down(void);
  void Left(void);
  void Right(void);
  explicit operator bool(void) const;
  typedef memory::WeakPtr<Hero, class HeroImpl> WeakPtr;
private:
  std::shared_ptr<class HeroImpl> impl_;
  friend WeakPtr;
};
}
#endif