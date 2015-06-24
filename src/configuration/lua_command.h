#ifndef LUA_COMMAND_H_
#define LUA_COMMAND_H_
#include <memory>
namespace lua
{
class StackImpl;
typedef std::shared_ptr<StackImpl> StackPtr;
class Command
{
public:
  Command(StackPtr const& stack);
  auto operator()() -> bool;
private:
  class Impl;
  std::shared_ptr<Impl> impl_;
};
}
#endif