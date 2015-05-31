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
  Impl(json::JSON const& json, display::Window& window, event::Queue& queue, dynamics::World& world, collision::Group& collision, int& plane, boost::filesystem::path const& path);
  void Init(Scene& scene, dynamics::World& world, int plane);
  void Render();
  void Pause();
  void Resume();
  void Body(std::string const& expression, bool left_facing);
  void Body(std::string const& expression);
  void Body(bool left_facing);
  void Eyes(std::string const& expression);
  void Mouth(std::string const& expression);
  void Mouth(int open);
  void Position(game::Position const& position);
  game::Position Position();
  void Velocity(game::Position const& velocity);
  game::Position Velocity() const;
  void Force(game::Position const& force);
  void Impulse(game::Position const& impulse);
  void Modulation(float r, float g, float b, float a);
  void Begin();
  void End();
  void Blink();
  void Next();
  double Dilation() const;
  void Dilation(double dilation);

  game::Position position_;
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
};
}
#endif