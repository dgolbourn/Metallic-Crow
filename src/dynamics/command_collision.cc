#include "command_collision.h"
#include "collision_group.h"
#include "channel.h"
#include "make_channel.h"
namespace game
{
class CommandCollisionImpl
{
public:
  CommandCollisionImpl(Collision const& collision);
  void Add(int group, dynamics::Body const& body, event::Command const& command, bool start);
  void Link(int group_a, int group_b);
  CollisionGroup<dynamics::Channel, true> start_;
  CollisionGroup<dynamics::Channel, false> end_;
};

CommandCollisionImpl::CommandCollisionImpl(Collision const& collision) : start_(collision), end_(collision)
{
}

void CommandCollisionImpl::Add(int group, dynamics::Body const& body, event::Command const& command, bool start)
{
  if(start)
  {
    start_.Add(group, body, dynamics::MakeChannel(command));
    end_.Add(group, body, dynamics::MakeChannel());
  }
  else
  {
    start_.Add(group, body, dynamics::MakeChannel());
    end_.Add(group, body, dynamics::MakeChannel(command));
  }
}

void CommandCollisionImpl::Link(int group_a, int group_b)
{
  start_.Link(group_a, group_b);
  end_.Link(group_a, group_b);
}

void CommandCollision::Add(int group, dynamics::Body const& body, event::Command const& command, bool start)
{
  impl_->Add(group, body, command, start);
}

void CommandCollision::Link(int group_a, int group_b)
{
  impl_->Link(group_a, group_b);
}

CommandCollision::CommandCollision(Collision const& collision)
{
  impl_ = std::make_shared<CommandCollisionImpl>(collision);
}
}