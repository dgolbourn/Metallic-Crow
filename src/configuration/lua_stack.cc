#include "lua_stack.h"
#include "lua_exception.h"
#include "lua.hpp"
#include <sstream>
#include "log.h"
#include "lua_stack_impl.h"
#include "lua_command.h"
#include "lua_data.h"
namespace
{
auto Path(lua_State* state, boost::filesystem::path const& path) -> void
{
  std::stringstream stream;
  stream << (path / "?").string() << ";" << (path / "?.lua").string();
  std::string string = stream.str();

  lua_getglobal(state, "package");
  lua_pushstring(state, string.c_str());
  lua_setfield(state, -2, "path");
  lua_pop(state, 1);
}

auto WeakRegistry(lua_State* state) -> int
{
  lua_newtable(state);
  lua_newtable(state);
  lua_pushliteral(state, "__mode");
  lua_pushliteral(state, "v");
  lua_rawset(state, -3);
  lua_setmetatable(state, -2);
  return luaL_ref(state, LUA_REGISTRYINDEX);
}

auto PopException(lua_State* state) -> void
{
  std::exception_ptr exception;
  lua::Get(state, exception);
  if(exception)
  {
    lua_pop(state, 1);
    std::rethrow_exception(exception);
  }
}

auto PushToLibrary(lua_State* state, std::string const& function, std::string const& library) -> void
{
  lua_getglobal(state, "package");
  lua_getfield(state, -1, "loaded");
  lua_getfield(state, -1, library.c_str());
  if(!lua_istable(state, -1))
  {
    lua_pop(state, 1);
    lua_newtable(state);
    lua_pushvalue(state, -1);
    lua_setfield(state, -3, library.c_str());
  }
  lua_pushvalue(state, -4);
  lua_setfield(state, -2, function.c_str());
  lua_pop(state, 4);
}

auto InitState(boost::filesystem::path const& path) -> lua_State* 
{
  lua_State* state = luaL_newstate();
  if(state == nullptr)
  {
    BOOST_THROW_EXCEPTION(lua::Exception());
  }

  if(lua_checkstack(state, 256) == 0)
  {
    BOOST_THROW_EXCEPTION(lua::Exception());
  }

  luaL_openlibs(state);
  
  Path(state, path);

  lua::Init<std::exception_ptr>(state);

  return state;
}

auto Event(lua_State* state) noexcept -> int
{
  try
  {
    lua::StackImpl* ptr;
    if(lua_islightuserdata(state, lua_upvalueindex(1)))
    {
      ptr = static_cast<lua::StackImpl*>(lua_touserdata(state, lua_upvalueindex(1)));
    }
    else
    {
      BOOST_THROW_EXCEPTION(lua::Exception());
    }

    std::string name;
    if(lua_isstring(state, lua_upvalueindex(2)))
    {
      name = std::string(lua_tostring(state, lua_upvalueindex(2)));
    }
    else
    {
      BOOST_THROW_EXCEPTION(lua::Exception());
    }

    int out;
    if(lua_isnumber(state, lua_upvalueindex(3)))
    {
      out = static_cast<int>(lua_tointeger(state, lua_upvalueindex(3)));
    }
    else
    {
      BOOST_THROW_EXCEPTION(lua::Exception());
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
    lua::Push(state, std::current_exception());
  }
  return lua_error(state);
}
}

namespace lua
{
StackImpl::StackImpl(boost::filesystem::path const& path) : state_(InitState(path))
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

auto StackImpl::Load(boost::filesystem::path const& file) -> void
{
  int ret = luaL_dofile(state_, file.string().c_str());
  if(ret)
  {
    PopException(state_);
 
    char const* error = lua_tostring(state_, -1);
    if(error != nullptr)
    {
      std::string error(error);
      lua_pop(state_, 1);
      BOOST_THROW_EXCEPTION(Exception()
        << Exception::What(error)
        << Exception::Code(ret));
    }
    else
    {
      BOOST_THROW_EXCEPTION(Exception());
    }
  }
}

auto StackImpl::Release() -> void
{
  Guard* guard = *stack_.begin();
  stack_.pop_front();
  guard->impl_ = nullptr;
}

auto StackImpl::Call(int in, int out) -> void
{
  for(int i = 0; i <= in; ++i)
  {
    Release();
  }

  int ret = lua_pcall(state_, in, out, 0);

  if(ret)
  {
    PopException(state_);

    char const* error = lua_tostring(state_, -1);
    if(error != nullptr)
    {
      std::string error(error);
      lua_pop(state_, 1);
      BOOST_THROW_EXCEPTION(Exception()
        << Exception::What(error)
        << Exception::Code(ret));
    }
    else
    {
      BOOST_THROW_EXCEPTION(Exception());
    }
  }
}

auto StackImpl::Pop(event::Command& out) -> void
{
  Release();

  if(!lua_isfunction(state_, -1))
  {
    lua_pop(state_, 1);
    BOOST_THROW_EXCEPTION(Exception());
  }
  
  lua_pushvalue(state_, -1);
  lua_pop(state_, 1);
  out = Command(shared_from_this());
}

auto StackImpl::Pop(int& out) -> void
{
  Release();

  int ret;
  out = static_cast<int>(lua_tointegerx(state_, -1, &ret)); 
  lua_pop(state_, 1);
  if(!ret)
  {
    BOOST_THROW_EXCEPTION(Exception());
  }
}

auto StackImpl::Pop(float& out) -> void
{
  Release();

  int ret;
  out = static_cast<float>(lua_tonumberx(state_, -1, &ret));
  lua_pop(state_, 1);
  if(!ret)
  {
    BOOST_THROW_EXCEPTION(Exception());
  }
}

auto StackImpl::Pop(double& out) -> void
{
  Release();

  int ret;
  out = lua_tonumberx(state_, -1, &ret);
  lua_pop(state_, 1);
  if(!ret)
  {
    BOOST_THROW_EXCEPTION(Exception());
  }
}

auto StackImpl::Pop(std::string& out) -> void
{
  Release();

  if(!lua_isstring(state_, -1))
  {
    lua_pop(state_, 1);
    BOOST_THROW_EXCEPTION(Exception());
  }
  out = std::string(lua_tostring(state_, -1));
  lua_pop(state_, 1);
}

auto StackImpl::Pop() -> void
{
  Release();
  lua_pop(state_, 1);
}

auto StackImpl::Pop(bool& out) -> void
{
  Release();

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

auto StackImpl::Add(event::Command const& command, std::string const& name, int out, std::string const& library) -> void
{
  lua_pushlightuserdata(state_, static_cast<void*>(this));
  lua_pushstring(state_, name.c_str());
  lua_pushinteger(state_, static_cast<lua_Integer>(out));
  lua_pushcclosure(state_, Event, 3);
  PushToLibrary(state_, name, library);
  if(!map_.emplace(name, command).second)
  {
    BOOST_THROW_EXCEPTION(Exception());
  }
}

auto StackImpl::Collect(int size) -> void
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

auto StackImpl::Pause() -> void
{
  lua_gc(state_, LUA_GCSTOP, 0);
}

auto StackImpl::Resume() -> void
{
  lua_gc(state_, LUA_GCRESTART, 0);
}

auto Stack::Load(boost::filesystem::path const& file) -> void
{
  impl_->Load(file);
}

auto Stack::Call(int in, int out) -> std::vector<Guard> 
{
  impl_->Call(in, out);

  std::vector<Guard> guard;
  guard.reserve(out);
  for(int i = 0; i < out; ++i)
  {
    guard.push_back(Guard(impl_.get()));
  }
  return guard;
}

auto Stack::Pop(int& out) -> void
{
  impl_->Pop(out);
}

auto Stack::Pop(float& out) -> void
{
  impl_->Pop(out);
}

auto Stack::Pop(double& out) -> void
{
  impl_->Pop(out);
}

auto Stack::Pop(std::string& out) -> void
{
  impl_->Pop(out);
}

auto Stack::Pop(bool& out) -> void
{
  impl_->Pop(out);
}

auto Stack::Pop(event::Command& out) -> void
{
  impl_->Pop(out);
}

auto Stack::Pop() -> void
{
  impl_->Pop();
}

auto Stack::Get(std::string const& global) -> Guard 
{
  lua_getglobal(impl_->state_, global.c_str());
  return Guard(impl_.get());
}

auto Stack::Get(int index) -> Guard
{
  lua_pushvalue(impl_->state_, index);
  return Guard(impl_.get());
}

auto Stack::Field(std::string const& field) -> Guard
{
  lua_getfield(impl_->state_, -1, field.c_str());
  return Guard(impl_.get());
}

auto Stack::Push(int in) -> Guard
{
  lua_pushinteger(impl_->state_, in);
  return Guard(impl_.get());
}

auto Stack::Push(float in) -> Guard
{
  lua_pushnumber(impl_->state_, static_cast<lua_Number>(in));
  return Guard(impl_.get());
}

auto Stack::Push(double in) -> Guard 
{
  lua_pushnumber(impl_->state_, static_cast<lua_Number>(in));
  return Guard(impl_.get());
}

auto Stack::Push(std::string const& in) -> Guard
{
  lua_pushstring(impl_->state_, in.c_str());
  return Guard(impl_.get());
}

auto Stack::Push(bool in) -> Guard
{
  lua_pushboolean(impl_->state_, static_cast<int>(in));
  return Guard(impl_.get());
}

auto Stack::Add(event::Command const& command, std::string const& name, int out, std::string const& library) -> void
{
  impl_->Add(command, name, out, library);
}

auto Stack::Collect(int size) -> void
{
  impl_->Collect(size);
}

auto Stack::Pause() -> void
{
  impl_->Pause();
}

auto Stack::Resume() -> void
{
  impl_->Resume();
}

auto Stack::Check() -> bool
{
  return !lua_isnoneornil(impl_->state_, -1);
}

auto Stack::Field(int index) -> Guard
{
  lua_rawgeti(impl_->state_, -1, index);
  return Guard(impl_.get());
}

auto Stack::Size() -> int
{
  return lua_rawlen(impl_->state_, -1);
}

Stack::Stack(boost::filesystem::path const& path) : impl_(std::make_shared<StackImpl>(path))
{
}

Stack::operator lua_State*() const
{
  return impl_->state_;
}
}