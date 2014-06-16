#include "lua_stack.h"
#include "lua_exception.h"
#include "lua.hpp"
#include <map>
namespace lua
{
typedef std::map<std::string, event::Command> CommandMap;

class StackImpl
{
public:
  StackImpl(void);
  ~StackImpl(void);
  void Load(std::string const& file);
  void Call(int in, int out);
  void Pop(int& out, bool front);
  void Pop(float& out, bool front);
  void Pop(double& out, bool front);
  void Pop(std::string& out, bool front);
  void Pop(bool& out, bool front);
  void Add(event::Command const& command, std::string const& name, int out);
  lua_State* state_;
  CommandMap map_;
};

lua_State* Init(void)
{
  lua_State* state = luaL_newstate();
  if(state == nullptr)
  {
    BOOST_THROW_EXCEPTION(Exception());
  }
  return state;
}

StackImpl::StackImpl(void) : state_(Init())
{
}

StackImpl::~StackImpl(void)
{
  lua_close(state_);
}

void StackImpl::Load(std::string const& file)
{
  int ret = luaL_dofile(state_, file.c_str());
  if(ret)
  {
    std::string error(lua_tostring(state_, -1));
    lua_pop(state_, 1);
    BOOST_THROW_EXCEPTION(Exception()
      << Exception::What(error)
      << Exception::Code(ret));
  }
}

void StackImpl::Call(int in, int out)
{
  int ret = lua_pcall(state_, in, out, 0);
  if(ret)
  {
    std::string error(lua_tostring(state_, -1));
    lua_pop(state_, 1);
    BOOST_THROW_EXCEPTION(Exception()
      << Exception::What(error)
      << Exception::Code(ret));
  }
}

static int Index(bool front)
{
  int index;
  if(front)
  {
    index = 1;
  }
  else
  {
    index = -1;
  }
  return index;
}

void StackImpl::Pop(int& out, bool front)
{
  int ret;
  int index = Index(front);
  out = (int)lua_tointegerx(state_, index, &ret); 
  lua_remove(state_, index);
  if(!ret)
  {
    BOOST_THROW_EXCEPTION(Exception());
  }
}

void StackImpl::Pop(float& out, bool front)
{
  int ret;
  int index = Index(front);
  out = (float)lua_tonumberx(state_, index, &ret);
  lua_remove(state_, index);
  if(!ret)
  {
    BOOST_THROW_EXCEPTION(Exception());
  }
}

void StackImpl::Pop(double& out, bool front)
{
  int ret;
  int index = Index(front);
  out = lua_tonumberx(state_, index, &ret);
  lua_remove(state_, index);
  if(!ret)
  {
    BOOST_THROW_EXCEPTION(Exception());
  }
}

void StackImpl::Pop(std::string& out, bool front)
{
  int index = Index(front);
  if(!lua_isstring(state_, index))
  {
    lua_remove(state_, index);
    BOOST_THROW_EXCEPTION(Exception());
  }
  out = std::string(lua_tostring(state_, index));
  lua_remove(state_, index);
}

void StackImpl::Pop(bool& out, bool front)
{
  int index = Index(front);
  if(!lua_isboolean(state_, index))
  {
    lua_remove(state_, index);
    BOOST_THROW_EXCEPTION(Exception());
  }
  if(lua_toboolean(state_, index))
  {
    out = true;
  }
  else
  {
    out = false;
  }
  lua_remove(state_, index);
}

static int Event(lua_State* state)
{
  StackImpl* ptr;
  if(lua_islightuserdata(state, lua_upvalueindex(1)))
  {
    ptr = (StackImpl*)lua_touserdata(state, lua_upvalueindex(1));
  }
  else
  {
    BOOST_THROW_EXCEPTION(Exception());
  }

  std::string name;
  if(lua_isstring(state, lua_upvalueindex(2)))
  {
    name = std::string(lua_tostring(state, lua_upvalueindex(2)));
  }
  else
  {
    BOOST_THROW_EXCEPTION(Exception());
  }

  int out;
  if(lua_isnumber(state, lua_upvalueindex(3)))
  {
    out = (int)lua_tointeger(state, lua_upvalueindex(3));
  }
  else
  {
    BOOST_THROW_EXCEPTION(Exception());
  }

  auto iter = ptr->map_.find(name);
  if(iter != ptr->map_.end())
  {
    if(!iter->second())
    {
      ptr->map_.erase(iter);
    }
  }
  return out;
}

void StackImpl::Add(event::Command const& command, std::string const& name, int out)
{
  lua_pushlightuserdata(state_, (void*)this);
  lua_pushstring(state_, name.c_str());
  lua_pushinteger(state_, (lua_Integer)out);
  lua_pushcclosure(state_, Event, 3);
  lua_setglobal(state_, name.c_str());
  if(!map_.emplace(name, command).second)
  {
    BOOST_THROW_EXCEPTION(Exception());
  }
}

void Stack::Load(std::string const& file)
{
  impl_->Load(file);
}

void Stack::Call(int in, int out)
{
  impl_->Call(in, out);
}

void Stack::PopBack(int& out)
{
  impl_->Pop(out, false);
}

void Stack::PopBack(float& out)
{
  impl_->Pop(out, false);
}

void Stack::PopBack(double& out)
{
  impl_->Pop(out, false);
}

void Stack::PopBack(std::string& out)
{
  impl_->Pop(out, false);
}

void Stack::PopBack(bool& out)
{
  impl_->Pop(out, false);
}

void Stack::PopFront(int& out)
{
  impl_->Pop(out, true);
}

void Stack::PopFront(float& out)
{
  impl_->Pop(out, true);
}

void Stack::PopFront(double& out)
{
  impl_->Pop(out, true);
}

void Stack::PopFront(std::string& out)
{
  impl_->Pop(out, true);
}

void Stack::PopFront(bool& out)
{
  impl_->Pop(out, true);
}

void Stack::Get(std::string const& global)
{
  lua_getglobal(impl_->state_, global.c_str());
}

void Stack::Push(int in)
{
  lua_pushinteger(impl_->state_, in);
}

void Stack::Push(float in)
{
  lua_pushnumber(impl_->state_, lua_Number(in));
}

void Stack::Push(double in)
{
  lua_pushnumber(impl_->state_, lua_Number(in));
}

void Stack::Push(std::string const& in)
{
  lua_pushstring(impl_->state_, in.c_str());
}

void Stack::Push(bool in)
{
  lua_pushboolean(impl_->state_, int(in));
}

void Stack::Add(event::Command const& command, std::string const& name, int out)
{
  impl_->Add(command, name, out);
}

Stack::Stack(void)
{
  impl_ = std::make_shared<StackImpl>();
}
}