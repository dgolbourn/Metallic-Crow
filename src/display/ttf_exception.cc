#include "ttf_exception.h"
#include "SDL_ttf.h"
namespace ttf
{
auto Error() -> std::string
{
  return TTF_GetError();
}
}