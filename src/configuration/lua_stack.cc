#include "lua_stack.h"
#include "lua_exception.h"
#include "lua.hpp"
#include <sstream>
#include "log.h"
#include "lua_stack_impl.h"
#include "lua_command.h"
namespace lua
{
static void Path(lua_State* state, boost::filesystem::path const& path)
{
  std::stringstream stream;
  stream << (path / "?").string() << ";" << (path / "?.lua").string();
  std::string string = stream.str();

  lua_getglobal(state, "package");
  lua_pushstring(state, string.c_str());
  lua_setfield(state, -2, "path");
  lua_pop(state, 1);
}

static int WeakRegistry(lua_State* state)
{
  lua_newtable(state);
  lua_newtable(state);
  lua_pushliteral(state, "__mode");
  lua_pushliteral(state, "v");
  lua_rawset(state, -3);
  lua_setmetatable(state, -2);
  return luaL_ref(state, LUA_REGISTRYINDEX);
}

static void PushException(lua_State* state) noexcept
{
  std::exception_ptr* ptr = (std::exception_ptr*)lua_newuserdata(state, sizeof(std::exception_ptr));
  luaL_getmetatable(state, "exception");
  lua_setmetatable(state, -2);
  ptr = new (ptr)std::exception_ptr;
  *ptr = std::current_exception();
}

static void PopException(lua_State* state)
{
  std::exception_ptr* ptr = (std::exception_ptr*)luaL_testudata(state, -1, "exception");
  if(ptr != nullptr)
  {
    std::exception_ptr exception = std::move(*ptr);
    lua_pop(state, 1);
    std::rethrow_exception(exception);
  }
}

static int FinaliseException(lua_State *state) 
{
  ((std::exception_ptr*)lua_touserdata(state, 1))->~exception_ptr();
  return 0;
}

static void InitException(lua_State* state)
{
  luaL_newmetatable(state, "exception");

  lua_pushliteral(state, "__gc");
  lua_pushcfunction(state, FinaliseException);
  lua_rawset(state, -3);

  lua_pushliteral(state, "__-1");
  lua_pushvalue(state, -2);
  lua_rawset(state, -3);

  lua_pop(state, 1);
}

static lua_State* Init(boost::filesystem::path const& path)
{
  lua_State* state = luaL_newstate();
  if(state == nullptr)
  {
    BOOST_THROW_EXCEPTION(Exception());
  }

  luaL_openlibs(state);
  
  Path(state, path);

  InitException(state);

  return state;
}

StackImpl::StackImpl(boost::filesystem::path const& path) : state_(Init(path))
{
  weak_registry_ = WeakRegistry(state_);
  Pause();
}

StackImpl::~StackImpl()
{
  lua_close(state_);

  for(auto guard : stack_)
  {
    guard->impl_ = nullptr;
  }
}

void StackImpl::Load(boost::filesystem::path const& file)
{
  int ret = luaL_dofile(state_, file.string().c_str());
  if(ret)
  {
    PopException(state_);
 
    std::string error(lua_tostring(state_, -1));
    lua_pop(state_, 1);
    BOOST_THROW_EXCEPTION(Exception()
      << Exception::What(error)
      << Exception::Code(ret));
  }
}

void StackImpl::Call(std::string const& call, int in, int out)
{
  lua_getglobal(state_, call.c_str());
  for(int i = 0; i < in; ++i)
  {
    lua_pushvalue(state_, -in - 1);
  }
  
  int ret = lua_pcall(state_, in, out, 0);
  if(ret)
  {
    PopException(state_);

    std::string error(lua_tostring(state_, -1));
    lua_pop(state_, 1);
    BOOST_THROW_EXCEPTION(Exception()
      << Exception::What(error)
      << Exception::Code(ret));
  }
}

void StackImpl::Pop(event::Command& out)
{
  Guard* guard = *stack_.begin();
  stack_.pop_front();
  guard->impl_ = nullptr;

  if(!lua_isfunction(state_, -1))
  {
    lua_pop(state_, 1);
    BOOST_THROW_EXCEPTION(Exception());
  }
  
  lua_pushvalue(state_, -1);
  lua_pop(state_, 1);
  out = Command(shared_from_this());
}

void StackImpl::Pop(int& out)
{
  Guard* guard = *stack_.begin();
  stack_.pop_front();
  guard->impl_ = nullptr;

  int ret;
  out = (int)lua_tointegerx(state_, -1, &ret); 
  lua_pop(state_, 1);
  if(!ret)
  {
    BOOST_THROW_EXCEPTION(Exception());
  }
}

void StackImpl::Pop(float& out)
{
  Guard* guard = *stack_.begin();
  stack_.pop_front();
  guard->impl_ = nullptr;

  int ret;
  out = (float)lua_tonumberx(state_, -1, &ret);
  lua_pop(state_, 1);
  if(!ret)
  {
    BOOST_THROW_EXCEPTION(Exception());
  }
}

void StackImpl::Pop(double& out)
{
  Guard* guard = *stack_.begin();
  stack_.pop_front();
  guard->impl_ = nullptr;

  int ret;
  out = lua_tonumberx(state_, -1, &ret);
  lua_pop(state_, 1);
  if(!ret)
  {
    BOOST_THROW_EXCEPTION(Exception());
  }
}

void StackImpl::Pop(std::string& out)
{
  Guard* guard = *stack_.begin();
  stack_.pop_front();
  guard->impl_ = nullptr;

  if(!lua_isstring(state_, -1))
  {
    lua_pop(state_, 1);
    BOOST_THROW_EXCEPTION(Exception());
  }
  out = std::string(lua_tostring(state_, -1));
  lua_pop(state_, 1);
}

void StackImpl::Pop()
{
  Guard* guard = *stack_.begin();
  stack_.pop_front();
  guard->impl_ = nullptr;

  lua_pop(state_, 1);
}

void StackImpl::Pop(bool& out)
{
  Guard* guard = *stack_.begin();
  stack_.pop_front();
  guard->impl_ = nullptr;

  if(!lua_isboolean(state_, -1))
  {
    lua_pop(state_, 1);
    BOOST_THROW_EXCEPTION(Exception());
  }
  if(lua_toboolean(state_, -1))
  {
    out = true;
  }
  else
  {
    out = false;
  }
  lua_pop(state_, 1);
}

static int Event(lua_State* state) noexcept
{
  try
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
  catch(...)
  {
    PushException(state);
  }
  return lua_error(state);
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

void StackImpl::Collect(int size)
{
  if(size >= 0)
  {
    lua_gc(state_, LUA_GCSTEP, size);
  }
  else
  {
    lua_gc(state_, LUA_GCCOLLECT, 0);
  }
}

void StackImpl::Pause()
{
  lua_gc(state_, LUA_GCSTOP, 0);
}

void StackImpl::Resume()
{
  lua_gc(state_, LUA_GCRESTART, 0);
}

void Stack::Load(boost::filesystem::path const& file)
{
  impl_->Load(file);
}

std::vector<Guard> Stack::Call(std::string const& call, int in, int out)
{
  impl_->Call(call, in, out);

  std::vector<Guard> guard;
  guard.reserve(out);
  for(int i = 0; i < out; ++i)
  {
    Guard temp(impl_.get());
    guard.push_back(std::move(temp));
  }
  return guard;
}

void Stack::Pop(int& out)
{
  impl_->Pop(out);
}

void Stack::Pop(float& out)
{
  impl_->Pop(out);
}

void Stack::Pop(double& out)
{
  impl_->Pop(out);
}

void Stack::Pop(std::string& out)
{
  impl_->Pop(out);
}

void Stack::Pop(bool& out)
{
  impl_->Pop(out);
}

void Stack::Pop(event::Command& out)
{
  impl_->Pop(out);
}

void Stack::Pop()
{
  impl_->Pop();
}

Guard Stack::Get(std::string const& global)
{
  lua_getglobal(impl_->state_, global.c_str());
  return Guard(impl_.get());
}

Guard Stack::Get(int index)
{
  lua_pushvalue(impl_->state_, index);
  return Guard(impl_.get());
}

Guard Stack::Field(std::string const& field)
{
  lua_getfield(impl_->state_, -1, field.c_str());
  return Guard(impl_.get());
}

Guard Stack::Push(int in)
{
  lua_pushinteger(impl_->state_, in);
  return Guard(impl_.get());
}

Guard Stack::Push(float in)
{
  lua_pushnumber(impl_->state_, lua_Number(in));
  return Guard(impl_.get());
}

Guard Stack::Push(double in)
{
  lua_pushnumber(impl_->state_, lua_Number(in));
  return Guard(impl_.get());
}

Guard Stack::Push(std::string const& in)
{
  lua_pushstring(impl_->state_, in.c_str());
  return Guard(impl_.get());
}

Guard Stack::Push(bool in)
{
  lua_pushboolean(impl_->state_, int(in));
  return Guard(impl_.get());
}

void Stack::Add(event::Command const& command, std::string const& name, int out)
{
  impl_->Add(command, name, out);
}

void Stack::Collect(int size)
{
  impl_->Collect(size);
}

void Stack::Pause()
{
  impl_->Pause();
}

void Stack::Resume()
{
  impl_->Resume();
}

bool Stack::Check()
{
  return !lua_isnil(impl_->state_, -1);
}

Guard Stack::Field(int index)
{
  lua_rawgeti(impl_->state_, -1, index);
  return Guard(impl_.get());
}

int Stack::Size()
{
  return lua_rawlen(impl_->state_, -1);
}

Stack::Stack(boost::filesystem::path const& path) : impl_(std::make_shared<StackImpl>(path))
{
}
}