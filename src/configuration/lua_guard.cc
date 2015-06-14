#include "lua_guard.h"
#include "lua_stack_impl.h"
namespace lua
{
Guard::Guard(Guard&& other) : impl_(other.impl_)
{
  if(other.impl_ != nullptr)
  {
    auto next = impl_->stack_.begin();
    while(*next != &other)
    {
      ++next;
    }
    *next = this;
    other.impl_ = nullptr;
  }
}

Guard& Guard::operator=(Guard&& other)
{
  if(impl_ != nullptr)
  {
    auto iter = impl_->stack_.before_begin();
    auto next = impl_->stack_.begin();
    int index = -1;
    while(*next != this)
    {
      iter = next++;
      --index;
    }
    impl_->stack_.erase_after(iter);
    lua_remove(impl_->state_, index);
  }

  impl_ = other.impl_;

  if(other.impl_ != nullptr)
  {
    auto next = impl_->stack_.begin();
    while(*next != &other)
    {
      ++next;
    }
    *next = this;
    other.impl_ = nullptr;
  }

  return *this;
}

Guard::Guard(StackImpl* impl) : impl_(impl)
{
  impl_->stack_.push_front(this);
}

Guard::~Guard()
{
  if(impl_ != nullptr)
  {
    auto iter = impl_->stack_.before_begin();
    auto next = impl_->stack_.begin();
    int index = -1;
    while(*next != this)
    {
      iter = next++;
      --index;
    }
    impl_->stack_.erase_after(iter);
    lua_remove(impl_->state_, index);
  }
}
}