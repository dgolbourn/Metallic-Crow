#include "img_exception.h"
#include "SDL_image.h"
namespace img
{
auto Error() -> std::string
{
  return IMG_GetError();
}
}