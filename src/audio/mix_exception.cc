#include "mix_exception.h"
#include "SDL_mixer.h"
namespace mix
{
auto Error() -> std::string
{
  return Mix_GetError();
}
}