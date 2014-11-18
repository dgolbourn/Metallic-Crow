#include "lua_command.h"
#include "lua_stack_impl.h"
#include "log.h"
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
  reference_ = luaL_ref(stack->state_, LUA_REGISTRYINDEX);
}

Command::Impl::~Impl()
{
  try
  {
    if(StackPtr stack = stack_.lock())
    {
      luaL_unref(stack->state_, LUA_REGISTRYINDEX, reference_);
    }
  }
  catch(...)
  {
    exception::Log("Swallowed Exception");
  }
}

bool Command::Impl::Notify()
{
  bool ret = false;
  if(StackPtr stack = stack_.lock())
  {
    lua_rawgeti(stack->state_, LUA_REGISTRYINDEX, reference_);
    stack->Call(0, 0);
    ret = true;
  }
  return ret;
}

Command::Command(StackPtr const& stack) : impl_(std::make_shared<Impl>(stack))
{
}

bool Command::operator()()
{
  return impl_->Notify();
}
}