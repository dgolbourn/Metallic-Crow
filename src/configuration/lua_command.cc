#include "lua_command.h"
#include "lua_stack_impl.h"
#include "log.h"
#include "lua_exception.h"
namespace lua
{
namespace
{
typedef std::weak_ptr<StackImpl> WeakPtr;
}

class Command::Impl
{
public:
  Impl(StackPtr const& stack);
  ~Impl();
  bool Notify();
  int reference_;
  WeakPtr stack_;
};

Command::Impl::Impl(StackPtr const& stack) : stack_(stack)
{
  lua_rawgeti(stack->state_, LUA_REGISTRYINDEX, stack->weak_registry_);
  lua_pushvalue(stack->state_, -2);
  reference_ = luaL_ref(stack->state_, -2);
  lua_pop(stack->state_, 2);
}

Command::Impl::~Impl()
{
  if(StackPtr stack = stack_.lock())
  {
    lua_rawgeti(stack->state_, LUA_REGISTRYINDEX, stack->weak_registry_);
    luaL_unref(stack->state_, -1, reference_);
    lua_pop(stack->state_, 1);
  }
}

bool Command::Impl::Notify()
{
  bool valid = false;
  if(StackPtr stack = stack_.lock())
  {
    lua_rawgeti(stack->state_, LUA_REGISTRYINDEX, stack->weak_registry_);
    lua_rawgeti(stack->state_, -1, reference_);

    valid = (lua_isnil(stack->state_, -1) == 0);
  
    if(valid)
    {
      int ret = lua_pcall(stack->state_, 0, 0, 0);
      if(ret)
      {
        std::string error(lua_tostring(stack->state_, -1));
        lua_pop(stack->state_, 2);
        BOOST_THROW_EXCEPTION(Exception() << Exception::What(error) << Exception::Code(ret));
      }
    }
    else
    {
      lua_pop(stack->state_, 1);
    }
    lua_pop(stack->state_, 1);
  }
  return valid;
}

Command::Command(StackPtr const& stack) : impl_(std::make_shared<Impl>(stack))
{
}

bool Command::operator()()
{
  return impl_->Notify();
}
}