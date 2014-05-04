#ifndef COMMAND_COLLISION_H_
#define COMMAND_COLLISION_H_
#include <memory>
#include "collision.h"
#include "body.h"
#include "command.h"
#include "collision_type.h"
namespace game
{
class CommandCollision
{
public:
  CommandCollision(void) = default;
  CommandCollision(Collision const& collision);
  void Add(dynamics::Type group, dynamics::Body const& body, event::Command const& command, bool start);
  void Link(dynamics::Type group_a, dynamics::Type group_b);
private:
  std::shared_ptr<class CommandCollisionImpl> impl_;
};
}
#endif