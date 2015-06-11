#ifndef LUA_STACK_IMPL_H_
#define LUA_STACK_IMPL_H_
#include "lua.hpp"
#include <map>
#include "boost/filesystem.hpp"
#include <memory>
#include "command.h"
#include <forward_list>
namespace lua
{
typedef std::map<std::string, event::Command> CommandMap;

class Guard;

class StackImpl final : public std::enable_shared_from_this<StackImpl>
{
public:
  StackImpl(boost::filesystem::path const& path);
  ~StackImpl();
  void Load(boost::filesystem::path const& file);
  void Call(std::string const& call, int in, int out);
  void Pop(int& out);
  void Pop(float& out);
  void Pop(double& out);
  void Pop(std::string& out);
  void Pop(bool& out);
  void Pop(event::Command& out);
  void Pop();
  void Add(event::Command const& command, std::string const& name, int out);
  void Collect(int size);
  void Pause();
  void Resume();
  lua_State* state_;
  CommandMap map_;
  int weak_registry_;
  std::forward_list<Guard*> stack_;
};
}
#endif