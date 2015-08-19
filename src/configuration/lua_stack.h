#ifndef LUA_STACK_H_
#define LUA_STACK_H_
#include <memory>
#include <string>
#include "command.h"
#include "boost/filesystem.hpp"
#include "lua_guard.h"
extern "C" 
{
typedef struct lua_State lua_State;
}

namespace lua
{
class Stack
{
public:
  Stack(boost::filesystem::path const& path);
  auto Load(boost::filesystem::path const& file) -> void;
  auto Call(int in, int out) -> std::vector<Guard>;
  auto Pop(int& out) -> void;
  auto Pop(float& out) -> void;
  auto Pop(double& out) -> void;
  auto Pop(std::string& out) -> void;
  auto Pop(bool& out) -> void;
  auto Pop(event::Command& out) -> void;
  auto Pop() -> void;
  auto Get(std::string const& global) -> Guard;
  auto Get(int index) -> Guard;
  auto Field(std::string const& field) -> Guard;
  auto Field(int index) -> Guard;
  auto Check() -> bool;
  auto Size() -> int;
  auto Push(int in) -> Guard;
  auto Push(float in) -> Guard;
  auto Push(double in) -> Guard;
  auto Push(std::string const& in) -> Guard;
  auto Push(bool in) -> Guard;
  auto Add(event::Command const& command, std::string const& name, int out, std::string const& library) -> void;
  auto Collect(int size) -> void;
  auto Pause() -> void;
  auto Resume() -> void;
  explicit operator lua_State*() const;
  template<class T> auto At(int index) -> T
  {
    Guard guard = Get(index);
    T value;
    Pop(value);
    return value;
  }
  template<class T> auto Field(std::string field) -> T
  {
    Guard guard = Field(field);
    T value;
    Pop(value);
    return value;
  }
    template<class T> auto Field(int index) -> T
  {
    Guard guard = Field(index);
    T value;
    Pop(value);
    return value;
  }
private:
  std::shared_ptr<class StackImpl> impl_;
};
}
#endif