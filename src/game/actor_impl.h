#ifndef ACTOR_IMPL_H_
#define ACTOR_IMPL_H_
#include "timer.h"
#include "game_body.h"
#include "feature.h"
namespace game
{
class Actor::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(lua::Stack& lua, display::Window& window, event::Queue& queue, dynamics::World& world, collision::Group& collision, boost::filesystem::path const& path, event::Timeslice& loader);
  ~Impl();
  auto Init(dynamics::World& world) -> void;
  auto Render() const -> void;
  auto Pause() -> void;
  auto Resume() -> void;
  auto Body(std::string const& expression, bool left_facing) -> void;
  auto Body(std::string const& expression) -> void;
  auto Body(bool left_facing) -> void;
  auto Eyes(std::string const& expression) -> void;
  auto Mouth(std::string const& expression) -> void;
  auto Mouth(int open) -> void;
  auto Position(game::Position const& position) -> void;
  auto Position() -> game::Position;
  auto Velocity(game::Position const& velocity) -> void;
  auto Velocity() const -> game::Position;
  auto Force(game::Position const& force) -> void;
  auto Impulse(game::Position const& impulse) -> void;
  auto Modulation(float r, float g, float b, float a) -> void;
  auto Begin() -> void;
  auto End() -> void;
  auto Blink() -> void;
  auto Next() -> void;
  auto Dilation() const -> double;
  auto Dilation(double dilation) -> void;
  auto Rotation() const -> double;
  auto Rotation(double angle) -> void;
  auto Scale(float scale) -> void;
  auto Scale() const -> float;
  auto Link(std::string const& group) -> void;
  auto Unlink(std::string const& group) -> void;

  game::Position position_;
  double angle_;
  display::Modulation modulation_;

  dynamics::Body dynamics_body_;
  game::Position force_;

  Feature mouth_;
  Feature eyes_;
  event::Timer blink_;
  bool open_;

  double dilation_;
  event::Timer animation_;
  game::Body game_body_;
  collision::Group collision_;
};
}
#endif