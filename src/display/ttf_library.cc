#include "ttf_library.h"
#include "SDL_ttf.h"
#include "ttf_exception.h"

namespace ttf
{
static int reference_count;

Library::Library()
{
  if(!TTF_WasInit() && TTF_Init()==-1) 
  {
    BOOST_THROW_EXCEPTION(Exception() << Exception::What(Error()));
  }
  ++reference_count;
}

Library::~Library()
{
  if(reference_count > 0)
  {
    --reference_count;
    if(reference_count == 0)
    {
      TTF_Quit();
    }
  }
}

Library::Library(Library const&) : Library()
{
}

Library::Library(Library&&) : Library()
{
}

Library& Library::operator=(Library)
{
  return *this;
}
}