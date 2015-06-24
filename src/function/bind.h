#ifndef BIND_H_
#define BIND_H_
#include "command.h"
#include <memory>
#include "boost/optional.hpp"
namespace function
{
template<class Method, class Shared, class... Args> auto Bind(Method&& method, Shared const& shared, Args... args) -> event::Command
{
  Shared::WeakPtr weak(shared);
  return [=]() mutable -> bool
  {
    bool locked = false;
    if(auto shared_locked = weak.Lock())
    {
      (shared_locked.*method)(args...);
      locked = true;
    }
    return locked;
  };
}

template<class Method, class Impl, class... Args> auto Bind(Method&& method, std::shared_ptr<Impl> const& shared, Args... args) -> event::Command
{
  std::weak_ptr<Impl> weak = shared;
  return [=]() -> bool
  {
    bool locked = false;
    if(auto shared_locked = weak.lock())
    {
      Impl& ptr = *shared_locked.get();
      (ptr.*method)(args...);
      locked = true;
    }
    return locked;
  };
}

template<class Impl, class... Args> auto Bind(void(Impl::*method)(Args...), std::shared_ptr<Impl> const& shared) -> std::function<bool(Args...)>
{
  std::weak_ptr<Impl> weak = shared;
  return [=](Args... args) -> bool
  {
    if(auto shared_locked = weak.lock())
    {
      Impl* ptr = shared_locked.get();
      (ptr->*method)(args...);
      return true;
    }
    else
    {
      return false;
    }
  };
}

template<class Return, class Impl, class... Args> auto Bind(Return(Impl::*method)(Args...), std::shared_ptr<Impl> const& shared) -> std::function<boost::optional<Return>(Args...)>
{
  std::weak_ptr<Impl> weak = shared;
  return [=](Args... args) -> boost::optional<Return>
  {
    if(auto shared_locked = weak.lock())
    {
      Impl* ptr = shared_locked.get();
      return (ptr->*method)(args...);
    }
    else
    {
      return boost::optional<Return>();
    }
  };
}

template<class Shared, class... Args> auto Bind(void(Shared::*method)(Args...), Shared const& shared) -> std::function<bool(Args...)>
{
  Shared::WeakPtr weak(shared);
  return [=](Args... args) -> bool
  {
    if(auto shared_locked = weak.Lock())
    {
      (shared_locked.*method)(args...);
      return true;
    }
    else
    { 
      return false;
    }
  };
}

template<class Return, class Shared, class... Args> auto Bind(Return(Shared::*method)(Args...), Shared const& shared) -> std::function<boost::optional<Return>(Args...)>
{
  Shared::WeakPtr weak(shared);
  return [=](Args... args) -> boost::optional<Return>
  {
    if(auto shared_locked = weak.Lock())
    {
      return (shared_locked.*method)(args...);
    }
    else
    { 
      return boost::optional<Return>();
    }
  };
}


template<class Method, class Impl, class... Args> auto Bind(Method&& method, std::weak_ptr<Impl> weak, Args... args) -> event::Command 
{
  return [=]() -> bool
  {
    bool locked = false;
    if(auto shared_locked = weak.lock())
    {
      Impl* ptr = shared_locked.get();
      (ptr->*method)(args...);
      locked = true;
    }
    return locked;
  };
}

template<class Return, class Impl, class... Args> auto Bind(Return(Impl::*method)(Args...), std::weak_ptr<Impl> weak) -> std::function<boost::optional<Return>(Args...)>
{
  return [=](Args... args) -> boost::optional<Return>
  {
    if(auto shared_locked = weak.lock())
    {
      Impl* ptr = shared_locked.get();
      return (ptr->*method)(args...);
    }
    else
    {
      return boost::optional<Return>();
    }
  };
}
}
#endif