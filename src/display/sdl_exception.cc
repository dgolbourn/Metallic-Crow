#include "sdl_exception.h"
#include "SDL.h"
namespace sdl
{
std::string Error()
{
  return SDL_GetError();
}
}