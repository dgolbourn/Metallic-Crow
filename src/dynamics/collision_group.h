#ifndef COMMAND_GROUP_H_
#define COMMAND_GROUP_H_
#include <memory>
#include <string>
#include "command.h"
#include "json.h"
#include "collision.h"
#include "body.h"
namespace collision
{
class Group
{
public:
  Group() = default;
  Group(json::JSON const& json, Collision const& collision);
  void Begin(std::string const& group_a, std::string const& group_b, event::Command const& command);
  void End(std::string const& group_a, std::string const& group_b, event::Command const& command);
  void Add(std::string const& group, dynamics::Body const& body);
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif