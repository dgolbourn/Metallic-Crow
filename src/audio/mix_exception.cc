#include "mix_exception.h"
#include "SDL_mixer.h"
namespace mix
{
std::string Error()
{
  return Mix_GetError();
}
}