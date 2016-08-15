#ifndef SDL_EXCEPTION_H_
#define SDL_EXCEPTION_H_
#include "exception.h"
#include <string>
namespace sdl
{
auto Error() -> std::string;

class Exception : virtual public exception::Exception
{
public:
  typedef boost::error_info<class What, std::string> What;
  char const* what() const noexcept {return "SDL Exception";}
};
}
#endif