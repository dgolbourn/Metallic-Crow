#ifndef MIX_LIBRARY_H_
#define MIX_LIBRARY_H_
#include "sdl_library.h"
namespace mix
{
class Library
{
public:
  Library();
  Library(Library const& other);
  Library(Library&& other);
  Library& operator=(Library other);
  ~Library();
private:
  sdl::Library sdl_;
};
}
#endif