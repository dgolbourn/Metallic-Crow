#ifndef LUA_DATA_H_
#define LUA_DATA_H_
#include "lua.hpp"
#include <typeinfo>
namespace lua
{
template<class T> auto Push(lua_State* state, T&& data) -> void
{
  auto ptr = lua_newuserdata(state, sizeof T);
  luaL_getmetatable(state, typeid(T).name());
  lua_setmetatable(state, -2);
  new (ptr) T(std::forward<T>(data));
}

template<class T> auto Get(lua_State* state, T& data) -> void
{
  auto ptr = luaL_testudata(state, -1, typeid(T).name());
  if(ptr != nullptr)
  {
    data = std::move(*static_cast<T*>(ptr));
  }
}

template<class T> auto Finalise(lua_State* state) noexcept -> int
{
  static_cast<T*>(lua_touserdata(state, 1))->~T();
  return 0;
}

template<class T> auto Init(lua_State* state) noexcept -> void
{
  luaL_newmetatable(state, typeid(T).name());
  lua_pushliteral(state, "__gc");
  lua_pushcfunction(state, Finalise<T>);
  lua_rawset(state, -3);
  lua_pushliteral(state, "__index");
  lua_pushvalue(state, -2);
  lua_rawset(state, -3);
  lua_pop(state, 1);
}
}
#endif