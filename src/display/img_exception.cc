#include "img_exception.h"
#include "SDL_image.h"
namespace img
{
std::string Error()
{
  return IMG_GetError();
}
}