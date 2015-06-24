#include "sdl_exception.h"
#include "SDL.h"
namespace sdl
{
auto Error() -> std::string
{
  return SDL_GetError();
}
}