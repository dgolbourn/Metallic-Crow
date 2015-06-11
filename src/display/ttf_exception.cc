#include "ttf_exception.h"
#include "SDL_ttf.h"
namespace ttf
{
std::string Error()
{
  return TTF_GetError();
}
}