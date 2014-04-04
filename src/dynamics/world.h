#ifndef WORLD_H_
#define WORLD_H_
#include <memory>
#include "json.h"
#include "weak_ptr.h"
#include "position.h"
#include "command.h"
namespace game
{
class Collision;
}

namespace dynamics
{
class Body
{
public:
  Body(void) = default;
  Body(json::JSON const& json, class World& world);
  Body(float x, float y, float u, float v, float w, float h, float m, float c, float d, float k, class World& world);
  explicit operator bool(void) const;
  bool operator<(Body const& other) const;
  game::Position Position(void) const;
  void Position(float x, float y);
  game::Position Velocity(void) const;
  void Velocity(float x, float y);
  void Force(float x, float y);
  typedef memory::WeakPtr<Body, class BodyImpl> WeakPtr;
private:
  std::shared_ptr<class BodyImpl> impl_;
  friend WeakPtr;
  friend class WorldImpl;
};

class World
{
public:
  World(void) = default;
  World(json::JSON const& json, game::Collision& collision);
  explicit operator bool(void) const;
  void Step(void);
  void Add(event::Command const& command);
  typedef memory::WeakPtr<World, class WorldImpl> WeakPtr;
private:
  std::shared_ptr<class WorldImpl> impl_;
  friend WeakPtr;
  friend class BodyImpl;
};
}
#endif