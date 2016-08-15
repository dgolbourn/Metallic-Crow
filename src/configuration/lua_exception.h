#ifndef LUA_EXCEPTION_H_
#define LUA_EXCEPTION_H_
#include "exception.h"
#include <string>
namespace lua
{
class Exception : virtual public exception::Exception
{
public:
  typedef boost::error_info<class What, std::string> What;
  typedef boost::error_info<class Code, int> Code;
  typedef boost::error_info<class Trace, std::string> Trace;
  char const* what() const noexcept {return "Lua Exception";}
};
}
#endif