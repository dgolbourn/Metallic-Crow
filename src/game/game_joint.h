#ifndef GAME_JOINT_H_
#define GAME_JOINT_H_
#include <memory>
#include "actor.h"
#include "json.h"
namespace game
{
class Joint
{
public:
  Joint(json::JSON const& json, Actor const& actor_a, Actor const& actor_b, dynamics::World& world);
private:
  std::shared_ptr<class JointImpl> impl_;
};
}
#endif