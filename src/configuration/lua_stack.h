#ifndef LUA_STACK_H_
#define LUA_STACK_H_
#include <memory>
#include <string>
#include "command.h"
#include "boost/filesystem.hpp"
namespace lua
{
class Stack
{
public:
  Stack(boost::filesystem::path const& path);
  void Load(boost::filesystem::path const& file);
  void Call(int in, int out);
  void PopBack(int& out);
  void PopBack(float& out);
  void PopBack(double& out);
  void PopBack(std::string& out);
  void PopBack(bool& out);
  void PopBack(event::Command& out);
  void PopFront(int& out);
  void PopFront(float& out);
  void PopFront(double& out);
  void PopFront(std::string& out);
  void PopFront(bool& out);
  void PopFront(event::Command& out);
  void Get(std::string const& global);
  void Push(int in);
  void Push(float in);
  void Push(double in);
  void Push(std::string const& in);
  void Push(bool in);
  void Add(event::Command const& command, std::string const& name, int out);
private:
  std::shared_ptr<class StackImpl> impl_;
};
}
#endif