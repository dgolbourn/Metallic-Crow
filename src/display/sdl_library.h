#ifndef SDL_LIBRARY_H_
#define SDL_LIBRARY_H_
#include "SDL_stdinc.h"
namespace sdl
{
class Library
{
public:
  Library(Uint32 flags);
  Library(Library const& other);
  Library(Library&& other);
  Library& operator=(Library other);
  ~Library();
private:
  Uint32 flags_;
};
}
#endif