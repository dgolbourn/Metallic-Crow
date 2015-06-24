#include "img_library.h"
#include "img_exception.h"
#include "SDL_image.h"
#include <climits>
namespace
{
int reference_count = 0;
}

namespace img
{
Library::Library(int flags)
{
  if((flags & IMG_Init(flags)) != flags)
  {
    BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
  }
  ++reference_count;
}

Library::~Library()
{
  --reference_count;
  if(reference_count == 0)
  {
    IMG_Quit();
  }
}

Library::Library(Library const&) : Library(0)
{
}

Library::Library(Library&&) : Library(0)
{
}

Library& Library::operator=(Library)
{
  return *this;
}
}