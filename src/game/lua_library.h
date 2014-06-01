#include "lua.hpp"
#include <memory>
#include <string>
namespace lua
{
class Library
{
public:
  Library(void);
  void Load(std::string const& file);
  void Call(int in, int out);
  void Pop(int& out);
  void Pop(float& out);
  void Pop(std::string& out);
  void Pop(bool& out);
  operator lua_State*(void) const;
private:
  std::shared_ptr<class LibraryImpl> impl_;
};
}