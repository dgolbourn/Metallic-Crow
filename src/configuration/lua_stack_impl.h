#ifndef LUA_STACK_IMPL_H_
#define LUA_STACK_IMPL_H_
#include "lua.hpp"
#include <map>
#include "boost/filesystem.hpp"
#include <memory>
#include "command.h"
namespace lua
{
typedef std::map<std::string, event::Command> CommandMap;

class StackImpl final : public std::enable_shared_from_this<StackImpl>
{
public:
  StackImpl(boost::filesystem::path const& path);
  ~StackImpl();
  void Load(boost::filesystem::path const& file);
  void Call(int in, int out);
  void Pop(int& out, bool front);
  void Pop(float& out, bool front);
  void Pop(double& out, bool front);
  void Pop(std::string& out, bool front);
  void Pop(bool& out, bool front);
  void Pop(event::Command& out, bool front);
  void Add(event::Command const& command, std::string const& name, int out);
  lua_State* state_;
  CommandMap map_;
};
}
#endif