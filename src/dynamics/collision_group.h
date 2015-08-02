#ifndef COLLISION_GROUP_H_
#define COLLISION_GROUP_H_
#include <memory>
#include <string>
#include "command.h"
#include "body.h"
#include "queue.h"
namespace collision
{
class Group
{
public:
  Group() = default;
  Group(event::Queue& queue);
  auto Begin(std::string const& group_a, std::string const& group_b, event::Command const& command) -> void;
  auto End(std::string const& group_a, std::string const& group_b, event::Command const& command) -> void;
  auto Link(std::string const& group_a, dynamics::Body const& body_a) -> void;
  auto Unlink(std::string const& group_a, dynamics::Body const& body_a) -> void;
  auto Link(std::string const& group_a, std::string const& group_b) -> void;
  auto Unlink(std::string const& group_a, std::string const& group_b) -> void;
  auto Check(dynamics::Body const& body_a, dynamics::Body const& body_b) -> bool;
  auto Begin(dynamics::Body const& body_a, dynamics::Body const& body_b) -> void;
  auto End(dynamics::Body const& body_a, dynamics::Body const& body_b) -> void;
  auto Unlink(dynamics::Body const& body_a) -> void;
  auto Unlink(std::string const& group_a) -> void;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif