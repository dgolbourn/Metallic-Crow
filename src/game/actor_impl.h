#ifndef ACTOR_IMPL_H_
#define ACTOR_IMPL_H_
#include "avatar.h"
#include "timer.h"
namespace game
{
class Actor::Impl final : public std::enable_shared_from_this<Impl>
{
public:
  Impl(json::JSON const& json, display::Window& window, event::Queue& queue, dynamics::World& world, collision::Group& collision, int& plane, boost::filesystem::path const& path);
  void Init(Scene& scene, dynamics::World& world, int plane);
  void Render(void) const;
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
  void Update(void);
  void Position(game::Position const& position);
  game::Position Position(void) const;
  void Velocity(game::Position const& velocity);
  game::Position Velocity(void) const;
  void Force(game::Position const& force);
  void Impulse(game::Position const& impulse);
  void Begin(void);
  void End(void);
  void Blink(void);

  Avatar avatar_;
  dynamics::Body body_;
  
  bool paused_;
  int x_sign_;
  int y_sign_;
  game::Position force_;
  float thrust_;

  event::Timer blink_;
  bool open_;
};
}
#endif