#ifndef LUA_STACK_H_
#define LUA_STACK_H_
#include <memory>
#include <string>
#include "command.h"
#include "boost/filesystem.hpp"
#include "lua_guard.h"
namespace lua
{
class Stack
{
public:
  Stack(boost::filesystem::path const& path);
  void Load(boost::filesystem::path const& file);
  std::vector<Guard> Call(int in, int out);
  void Pop(int& out);
  void Pop(float& out);
  void Pop(double& out);
  void Pop(std::string& out);
  void Pop(bool& out);
  void Pop(event::Command& out);
  void Pop();
  Guard Get(std::string const& global);
  Guard Get(int index);
  Guard Field(std::string const& field);
  Guard Field(int index);
  bool Check();
  int Size();
  Guard Push(int in);
  Guard Push(float in);
  Guard Push(double in);
  Guard Push(std::string const& in);
  Guard Push(bool in);
  void Add(event::Command const& command, std::string const& name, int out, std::string const& library);
  void Collect(int size);
  void Pause();
  void Resume();
private:
  std::shared_ptr<class StackImpl> impl_;
};
}
#endif