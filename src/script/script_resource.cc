#include "script_impl.h"
#include "bind.h"
namespace game
{
void Script::Impl::ResourceInit()
{
  lua_.Add(function::Bind(&Impl::SubtitleText, shared_from_this()), "resource_collect", 0);
}

void Script::Impl::ResourceCollect()
{
  std::string file;
  lua_.PopFront(file);
  if(file == "")
  {
    window_.Free();
    audio::Music::Free();
    audio::Sound::Free();
  }
  else
  {
    boost::filesystem::path file = file;
    window_.Free(file);
    audio::Music::Free(file);
    audio::Sound::Free(file);
  }
}
}