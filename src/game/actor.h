#ifndef ACTOR_H_
#define ACTOR_H_
#include <memory>
#include <string>
#include "window.h"
#include "position.h"
#include "weak_ptr.h"
#include "queue.h"
#include "collision_group.h"
#include "world.h"
#include "boost/filesystem.hpp"
#include "lua_stack.h"
#include "timeslice.h"
namespace game
{
class Actor
{
public:
  Actor() = default;
  Actor(lua::Stack& lua, display::Window& window, collision::Group& collision, event::Queue& queue, dynamics::World& world, boost::filesystem::path const& path, event::Timeslice& loader);
  auto Position(game::Position const& position) -> void;
  auto Position() const -> game::Position;
  auto Velocity(game::Position const& velocity) -> void;
  auto Velocity() const -> game::Position;
  auto Force(game::Position const& force) -> void;
  auto Impulse(game::Position const& impulse) -> void;
  auto Modulation(float r, float g, float b, float a) -> void;
  auto Pause() -> void;
  auto Resume() -> void;
  auto Body(std::string const& expression, bool left_facing) -> void;
  auto Body(std::string const& expression) -> void;
  auto Body(bool left_facing) -> void;
  auto Eyes(std::string const& expression) -> void;
  auto Mouth(std::string const& expression) -> void;
  auto Mouth(int open) -> void;
  auto Dilation() const -> double;
  auto Dilation(double dilation) -> void;
  auto Rotation(double angle) -> void;
  auto Rotation() const -> double;
  auto Scale(float scale) -> void;
  auto Scale() const -> float;
  auto Render() const -> void;
  auto Link(std::string const& group) -> void;
  auto Unlink(std::string const& group) -> void;
  auto Emit(float r, float g, float b) -> void;
  auto Intrinsic(float r, float g, float b) -> void;
  auto Active() const -> bool;
  explicit operator bool() const;
  bool operator==(Actor const& other) const;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
public:
  typedef memory::WeakPtr<Actor> WeakPtr;
  friend WeakPtr;
  friend class JointImpl;
  static size_t Hash(Actor const& actor);
};
}

namespace std 
{
template<> struct hash<game::Actor> 
{
  size_t operator()(game::Actor const& actor) const 
  { 
    return game::Actor::Hash(actor); 
  }
};
}

namespace boost 
{
template<> struct hash<game::Actor> 
{
  size_t operator()(game::Actor const& actor) const 
  { 
    return game::Actor::Hash(actor); 
  }
};
}
#endif