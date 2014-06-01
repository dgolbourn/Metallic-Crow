#include "lua_library.h"
#include "lua_exception.h"
namespace lua
{
class LibraryImpl
{
public:
  LibraryImpl(void);
  ~LibraryImpl(void);
  void Load(std::string const& file);
  void Call(int in, int out);
  void Pop(int& out);
  void Pop(float& out);
  void Pop(std::string& out);
  void Pop(bool& out);
  lua_State* state_;
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

LibraryImpl::LibraryImpl(void) : state_(Init())
{
}

LibraryImpl::~LibraryImpl(void)
{
  lua_close(state_);
}

void LibraryImpl::Load(std::string const& file)
{
  int ret = luaL_dofile(state_, file.c_str());
  if(ret)
  {
    std::string error(lua_tostring(state_, -1));
    lua_pop(state_, 1);
    BOOST_THROW_EXCEPTION(lua::Exception()
      << lua::Exception::What(error)
      << lua::Exception::Code(ret));
  }
}

void LibraryImpl::Call(int in, int out)
{
  int ret = lua_pcall(state_, in, out, 0);
  if(ret)
  {
    std::string error(lua_tostring(state_, -1));
    lua_pop(state_, 1);
    BOOST_THROW_EXCEPTION(lua::Exception()
      << lua::Exception::What(error)
      << lua::Exception::Code(ret));
  }
}

void LibraryImpl::Pop(int& out)
{
  int ret;
  out = (int)lua_tointegerx(state_, -1, &ret); 
  lua_pop(state_, 1);
  if(!ret)
  {
    BOOST_THROW_EXCEPTION(lua::Exception());
  }
}

void LibraryImpl::Pop(float& out)
{
  int ret;
  out = (float)lua_tonumberx(state_, -1, &ret);
  lua_pop(state_, 1);
  if(!ret)
  {
    BOOST_THROW_EXCEPTION(lua::Exception());
  }
}

void LibraryImpl::Pop(std::string& out)
{
  if(!lua_isstring(state_, -1))
  {
    lua_pop(state_, -1);
    BOOST_THROW_EXCEPTION(lua::Exception());
  }
  out = std::string(lua_tostring(state_, -1));
  lua_pop(state_, 1);
}

void LibraryImpl::Pop(bool& out)
{
  if(!lua_isboolean(state_, -1))
  {
    lua_pop(state_, -1);
    BOOST_THROW_EXCEPTION(lua::Exception());
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

Library::operator lua_State*(void) const
{
  return impl_->state_;
}

void Library::Load(std::string const& file)
{
  impl_->Load(file);
}

void Library::Call(int in, int out)
{
  impl_->Call(in, out);
}

void Library::Pop(int& out)
{
  impl_->Pop(out);
}

void Library::Pop(float& out)
{
  impl_->Pop(out);
}

void Library::Pop(std::string& out)
{
  impl_->Pop(out);
}

void Library::Pop(bool& out)
{
  impl_->Pop(out);
}

Library::Library(void)
{
  impl_ = std::make_shared<LibraryImpl>();
}
}