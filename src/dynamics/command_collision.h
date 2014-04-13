#ifndef COMMAND_COLLISION_H_
#define COMMAND_COLLISION_H_
#include <memory>
#include "collision.h"
#include "body.h"
#include "command.h"
namespace game
{
class CommandCollision
{
public:
  CommandCollision(void) = default;
  CommandCollision(Collision const& collision);
  void Add(int group, dynamics::Body const& body, event::Command const& command, bool start);
  void Link(int group_a, int group_b);
private:
  std::shared_ptr<class CommandCollisionImpl> impl_;
};
}
#endif